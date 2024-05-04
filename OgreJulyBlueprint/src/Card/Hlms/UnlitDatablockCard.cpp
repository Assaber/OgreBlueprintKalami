#include "Card/Hlms/UnlitDatablockCard.h"

#include "BKCreator.h"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKPushButton.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"
#include "unit/BKPixmap.h"
#include "unit/BKSpacer.h"
#include "unit/BKColorSelectorEx.h"
#include "BlueprintEditor.h"

#include "OgreRoot.h"
#include "OgreHlmsManager.h"

#include "Card/Hlms/BlendblockCard.h"
#include "Card/Hlms/MacroblockCard.h"
#include "OgreWidget.h"
#include "OgreItem.h"
#include "OgreSceneNode.h"

#include <QMessageBox>
#include <QUuid>
#include <QFileInfo>


#define CHANGE_DATABLOCK_NAME_HAS_NOTIFY 0

UnlitDatablockCard::UnlitDatablockCard()
    : mpUnlit(reinterpret_cast<Ogre::HlmsUnlit*>(Ogre::Root::getSingletonPtr()->getHlmsManager()->getHlms(Ogre::HLMS_UNLIT)))
{
    setTitle("Unlit数据块");

    mpNameLineEdit = BKCreator::create<BKLineEdit>();
    mpNameLineEdit->setText(mstrName.c_str())
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
        std::string name = param.toString().toStdString();
        if (name != mstrOldName)
        {
            mstrName = name;
            createHlms(true);
        }
        return false;
        });

    mpOutputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    mpOutputCell->setDataType(BKAnchor::Output, BKAnchor::String)
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("输出"), false)
        ->getAnchor(BKAnchor::AnchorType::Output)           // 皮一下 XD
        ->redirectToCard();

    BKCell* textureInputCell = BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None);
    BKAnchor* textureInputAnchor = textureInputCell->getAnchor(BKAnchor::Input);
    textureInputAnchor->setDateType(QMetaTypeId<UnlitDiffuseMapCard::Info>::qt_metatype_id());
    textureInputCell->append(BKCreator::create<BKLabel>()
        ->setText("材质", true)
        ->setDataChangeCallback([this, textureInputAnchor](const QVariant& param) -> bool {
            std::vector<QVariant> items;
            if (textureInputAnchor->getBindOutputData(items) < 0)
                return true;

            mTextureInfoSet.clear();
            for (const auto& item : items)
                mTextureInfoSet.insert(item.value<UnlitDiffuseMapCard::Info>());

            // 注意次序，不能留有空白
            // 详见OgreHlmsUnlitDatablock.h中关于diffuse_map2-16的说明
            if (mTextureInfoSet.size())
            {
                size_t total = mTextureInfoSet.size();
                auto lastItemItor = mTextureInfoSet.begin();
                lastItemItor = std::next(lastItemItor, total - 1);
                if (lastItemItor->index != total - 1)
                {
                    QMessageBox::warning(nullptr, "错误", "请注意漫反射贴图的索引：\n需要依次增加，不可留有空白");
                    mTextureInfoSet.clear();
                }
            }

            // 这里是需要重建材质的...吗？
            // todo...
            createHlms(true);
            return true;
            })
        );


    BKCell* blendblockCell = BKCreator::create(BKAnchor::Input);
    blendblockCell->append(BKCreator::create<BKLabel>()
        ->setText("混合", true)
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
            mBlendblock = param.value<Ogre::HlmsBlendblock>();
            createHlms(true);
            return true;
            })
    );
    blendblockCell->getAnchor(BKAnchor::Input)->setDateType(QMetaTypeId<Ogre::HlmsBlendblock>::qt_metatype_id());

    BKCell* macroblockCell = BKCreator::create(BKAnchor::Input);
    macroblockCell->append(BKCreator::create<BKLabel>()
        ->setText("宏", true)
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
            mMacroblock = param.value<Ogre::HlmsMacroblock>();
            createHlms(true);
            return true;
            })
    );
    macroblockCell->getAnchor(BKAnchor::Input)->setDateType(QMetaTypeId<Ogre::HlmsMacroblock>::qt_metatype_id());

    _pack({
        mpOutputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("名称")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(mpNameLineEdit),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("漫反射")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mDiffuse = param.value<QColor>();
                createHlms();
                return false;
                })),

        // 透明测试和动画先搁置
        // todo ...

        textureInputCell,
        blendblockCell,
        macroblockCell,
        });

    createHlms(true);
}

UnlitDatablockCard::~UnlitDatablockCard()
{
    // if (mpDatablock && mpDatablock != mpPbs->getDefaultDatablock())
    // {
    //     mpPbs->destroyDatablock(mpDatablock->getName());
    //     mpDatablock = nullptr;
    // }
}

QVariant UnlitDatablockCard::getCurrentCardValue()
{
    return QString(mstrName.c_str());
}

void UnlitDatablockCard::createHlms(bool recreate/* = false*/)
{
    bool notify = false;
    std::set<Ogre::SubItem*> refreshSubItems;

    // 我无法销毁一个datablock，因为别的渲染对象可能正在使用这个材质
    // 策略变更：只要名字发生变化，则重新创建datablock，当然也包括HlmsMacroblock和HlmsBlendblock的更新

    if (recreate)
    {
        mpDatablock = reinterpret_cast<Ogre::HlmsUnlitDatablock*>(mpUnlit->getDatablock(mstrName));

        Ogre::String uuid = "";
        while (mpDatablock)
        {
            uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
            mpDatablock = reinterpret_cast<Ogre::HlmsUnlitDatablock*>(mpUnlit->getDatablock(uuid));
        }

        if (!uuid.empty())
        {
#if CHANGE_DATABLOCK_NAME_HAS_NOTIFY
            int ret = QMessageBox::warning(nullptr, "警告", QString("存在重名数据块，名称存在更新：\n%1->%2\n确定将完成此更换，否则将推出数据块创建")
                .arg(mstrName.c_str())
                .arg(uuid.c_str()), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
            if (ret == QMessageBox::Cancel)
            {
                mstrName = mstrOldName;
                mpNameLineEdit->setText(mstrName.c_str());
                return;
            }
            else
            {
#endif
                mstrName = uuid;
                mpNameLineEdit->setText(mstrName.c_str());

                if (!mstrOldName.empty())
                {
                    // 移除使用旧纹理的对象
                    Ogre::SceneManager* sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager(OgreWidget::strSceneMgrName);
                    assert(sceneMgr && "what?");

                    getHitSubItems(mstrOldName, sceneMgr->getRootSceneNode(), refreshSubItems);
                    mpUnlit->destroyDatablock(mstrOldName);
                }
#if CHANGE_DATABLOCK_NAME_HAS_NOTIFY
            }
#endif
        }
        mstrOldName = mstrName;
    }

    if (!mpDatablock)
    {
        mpDatablock = reinterpret_cast<Ogre::HlmsUnlitDatablock*>(mpUnlit->createDatablock(mstrName, 
            mstrName, 
            mMacroblock,
            mBlendblock, 
            Ogre::HlmsParamVec()
        ));
        notify = true;
    }
        
    if (mTextureInfoSet.size())
    {
        for (const UnlitDiffuseMapCard::Info& item : mTextureInfoSet)
        {
            if (item.texture.empty())
                continue;

            mpDatablock->setTexture(item.index, item.texture);
            mpDatablock->setBlendMode(item.index, item.blendMode);
            if (item.uv > -1)
                mpDatablock->setTextureUvSource(item.index, item.uv);
        }
    }
    else
    {
        mpDatablock->setUseColour(true);
        mpDatablock->setColour(mDiffuse.isValid() ? BlueprintEditor::toColor(mDiffuse) : Ogre::ColourValue());
    }

    if (notify)
        mpOutputCell->valueChanged(QString(mstrName.c_str()));
}

void UnlitDatablockCard::getHitSubItems(const Ogre::IdString& id, Ogre::SceneNode* node, std::set<Ogre::SubItem*>& items)
{
    auto itemItorVec = node->getAttachedObjectIterator();
    auto itemItor = itemItorVec.begin();
    while (itemItor != itemItorVec.end())
    {
        Ogre::Item* item = dynamic_cast<Ogre::Item*>(*itemItor);
        if (item)
        {
            for (int i = 0; i < item->getNumSubItems(); ++i)
            {
                auto subItem = item->getSubItem(i);

                if (subItem->getDatablockOrMaterialName() == mstrOldName)
                {
                    subItem->setDatablock(mpUnlit->getDefaultDatablock());
                    items.insert(subItem);
                }
            }
        }
        ++itemItor;
    }

    auto nodeItorVec = node->getChildIterator();
    auto nodeItor = nodeItorVec.begin();
    while (nodeItor != nodeItorVec.end())
    {
        Ogre::SceneNode* item = dynamic_cast<Ogre::SceneNode*>(*nodeItor);
        if (item)
            getHitSubItems(id, item, items);

        ++nodeItor;
    }
}