#include "Card/Hlms/UnlitDatablockCard.h"

#include "BKCreator.hpp"
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
    setTitle("Unlit datablock");

    mpNameLineEdit = BKCreator::create<BKLineEdit>();
    mpNameLineEdit->setText(mstrName.c_str())
        ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
        QString qparam = param.toString();
        std::string name = qparam.toStdString();
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
            ->setText("Output"), false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    BKCell* textureInputCell = BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None);
    BKAnchor* textureInputAnchor = textureInputCell->getAnchor(BKAnchor::Input);
    textureInputAnchor->setDateType(QMetaTypeId<UnlitDiffuseMapCard::Info>::qt_metatype_id());
    textureInputCell->append(BKCreator::create<BKLabel>()
        ->setText("Material")
        ->setTitleNeverChanges(true)
        ->setDataChangeCallback([this, textureInputAnchor](BKUnit* unit, const QVariant& param) -> bool {
            std::vector<QVariant> items;
            if (textureInputAnchor->getBindOutputData(items) < 0)
                return true;

            mTextureInfoSet.clear();
            for (const auto& item : items)
                mTextureInfoSet.insert(item.value<UnlitDiffuseMapCard::Info>());

            // Pay attention to the order and leave no blanks
            // See See what is said about diffuse_map2~16 in OgreHlmsUnlitDatablock.h
            if (mTextureInfoSet.size())
            {
                size_t total = mTextureInfoSet.size();
                auto lastItemItor = mTextureInfoSet.begin();
                lastItemItor = std::next(lastItemItor, total - 1);
                if (lastItemItor->index != total - 1)
                {
                    QMessageBox::warning(nullptr, "Error", "Note that the index of the diffuse map: \nneeds to be increased sequentially, leaving no blanks");
                    mTextureInfoSet.clear();
                }
            }

            // Is this a material that needs to be rebuilt?
            // todo...
            createHlms(true);
            return true;
            })
        );


    BKCell* blendblockCell = BKCreator::create(BKAnchor::Input);
    blendblockCell->append(BKCreator::create<BKLabel>()
        ->setText("Blend")
        ->setTitleNeverChanges(true)
        ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
            mBlendblock = param.value<Ogre::HlmsBlendblock>();
            createHlms(true);
            return true;
            })
    );
    blendblockCell->getAnchor(BKAnchor::Input)->setDateType(QMetaTypeId<Ogre::HlmsBlendblock>::qt_metatype_id());

    BKCell* macroblockCell = BKCreator::create(BKAnchor::Input);
    macroblockCell->append(BKCreator::create<BKLabel>()
        ->setText("Macro")
        ->setTitleNeverChanges(true)
        ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
            mMacroblock = param.value<Ogre::HlmsMacroblock>();
            createHlms(true);
            return true;
            })
    );
    macroblockCell->getAnchor(BKAnchor::Input)->setDateType(QMetaTypeId<Ogre::HlmsMacroblock>::qt_metatype_id());

    _pack({
        mpOutputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Name")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(mpNameLineEdit),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Diffuse")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                mDiffuse = param.value<QColor>();
                createHlms();
                return false;
                })),

        // Transparency tests and animations are set aside
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
            int ret = QMessageBox::warning(nullptr, "Warning", QString("There is a duplicate name block, the name is updated:\n%1->%2\nOk will complete this replacement, otherwise block creation will be exited")
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
                    // Removes objects that use old textures
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
            if (item.uv > -1) {
                mpDatablock->setTextureUvSource(item.index, item.uv);
            }
        }
    }
    else
    {
        mpDatablock->setUseColour(true);
        mpDatablock->setColour(mDiffuse.isValid() ? BlueprintEditor::toColor(mDiffuse) : Ogre::ColourValue());
    }

    if (notify) {
        mpOutputCell->valueChanged(QString(mstrName.c_str()));
    }
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
        if (item) {
            getHitSubItems(id, item, items);
        }

        ++nodeItor;
    }
}