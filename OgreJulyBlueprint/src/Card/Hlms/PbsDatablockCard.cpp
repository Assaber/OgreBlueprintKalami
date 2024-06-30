#include "Card/Hlms/PbsDatablockCard.h"

#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKPushButton.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKColorSelectorEx.h"
#include "BlueprintEditor.h"

#include "OgreRoot.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"

#include "Card/Hlms/BlendblockCard.h"
#include "Card/Hlms/MacroblockCard.h"
#include "OgreWidget.h"
#include "OgreItem.h"
#include "OgreSceneNode.h"

#include <QMessageBox>
#include <QUuid>

#define CHANGE_DATABLOCK_NAME_HAS_NOTIFY 0

PbsDatablockCard::PbsDatablockCard()
    : mpPbs(reinterpret_cast<Ogre::HlmsPbs*>(Ogre::Root::getSingletonPtr()->getHlmsManager()->getHlms(Ogre::HLMS_PBS)))
{
    setTitle("Pbs datablock");

    mpNameLineEdit = BKCreator::create<BKLineEdit>();
    mpNameLineEdit->setText(mstrName.c_str())
        ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
        QString qname = param.toString();
        std::string name = qname.toStdString();
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

    auto texMapInputCell = BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None);
    texMapInputCell->append(BKCreator::create<BKLabel>()
        ->setText("Material")
        ->setTitleNeverChanges(true)
        ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
            std::vector<QVariant> items;
            if (mpTextureMapInputAnchor->getBindOutputData(items) < 0)
                return true;

            mTextureInfoSet.clear();
            for (const auto& item : items) {
                mTextureInfoSet.insert(item.value<PbsCommonTextureCard::Info>());
            }

            // Is this a material that needs to be rebuilt?
            // todo...
            createHlms(true);
            return true;
            })
        );
    mpTextureMapInputAnchor = texMapInputCell->getAnchor(BKAnchor::Input);
    mpTextureMapInputAnchor->setDateType(QMetaTypeId<PbsCommonTextureCard::Info>::qt_metatype_id());

    auto texDetailCell = BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None);
    texDetailCell->append(BKCreator::create<BKLabel>()
        ->setText("Detail")
        ->setTitleNeverChanges(true)
        ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
            std::vector<QVariant> items;
            if (mpTextureDetailAnchor->getBindOutputData(items) < 0)
                return true;

            mDetailInfoSet.clear();
            for (const auto& item : items) {
                mDetailInfoSet.insert(item.value<PbsDetailTextureCard::Info>());
            }

            // Is this a material that needs to be rebuilt?
            // todo...
            createHlms(true);
            return true;
            })
    );
    mpTextureDetailAnchor = texDetailCell->getAnchor(BKAnchor::Input);
    mpTextureDetailAnchor->setDateType(QMetaTypeId<PbsDetailTextureCard::Info>::qt_metatype_id());

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

        BKCreator::create(BKAnchor::AnchorType::None)->append({ BKCreator::create<BKLabel>()->setText("Background color")}),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>(BKColorSelectorEx::Type::Vector4)
            ->setColor(QColor())
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                mBackgroundColor = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Diffuse")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                mDiffuse = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Specular")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                mSpecular = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Roughness")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)
            ->setMinimum(0)
            ->setMaximum(1.0f)
            ->setCurrentValue(mRoughness)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                mRoughness = param.toFloat();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Emissive")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                mEmissive = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Fresnel")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                mFresnel = param.value<QColor>();
                createHlms();
                return false;
                })),

        texMapInputCell,
        texDetailCell,

        blendblockCell,
        macroblockCell,

        BKCreator::create(BKAnchor::AnchorType::Input)->append(
            BKCreator::create<BKLabel>()->setText("Transparent")
                ->setTitleNeverChanges(true)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& param) -> bool {
                    if (!param.canConvert<PbsTransparentCard::Info>())
                        return true;

                    mTransparentInfo = param.value<PbsTransparentCard::Info>();
                    createHlms();
                    return true;
                }))
            ->setDataType(BKAnchor::Input, QMetaTypeId<PbsTransparentCard::Info>::qt_metatype_id())
        });

    createHlms(true);
}

PbsDatablockCard::~PbsDatablockCard()
{
    // if (mpDatablock && mpDatablock != mpPbs->getDefaultDatablock())
    // {
    //     mpPbs->destroyDatablock(mpDatablock->getName());
    //     mpDatablock = nullptr;
    // }
}

QVariant PbsDatablockCard::getCurrentCardValue()
{
    return QString(mstrName.c_str());
}

void PbsDatablockCard::createHlms(bool recreate/* = false*/)
{
    bool notify = false;
    std::set<Ogre::SubItem*> refreshSubItems;

    if (recreate)
    {
        mpDatablock = reinterpret_cast<Ogre::HlmsPbsDatablock*>(mpPbs->getDatablock(mstrName));

        Ogre::String uuid = "";
        while (mpDatablock)
        {
            uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
            mpDatablock = reinterpret_cast<Ogre::HlmsPbsDatablock*>(mpPbs->getDatablock(uuid));
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
                    mpPbs->destroyDatablock(mstrOldName);
                }
#if CHANGE_DATABLOCK_NAME_HAS_NOTIFY
            }
#endif
        }
        mstrOldName = mstrName;
    }

    if (!mpDatablock)
    {
        mpDatablock = reinterpret_cast<Ogre::HlmsPbsDatablock*>(mpPbs->createDatablock(mstrName, 
            mstrName, 
            mMacroblock,
            mBlendblock, 
            Ogre::HlmsParamVec()
        ));
        notify = true;
    }
        
    mpDatablock->setBackgroundDiffuse(mBackgroundColor.isValid() ? BlueprintEditor::toColor(mBackgroundColor) : Ogre::ColourValue());
    mpDatablock->setDiffuse(mDiffuse.isValid() ? BlueprintEditor::toVec3f(mDiffuse) : Ogre::Vector3(1.0f, 1.0f, 1.0f));
    {
        mpDatablock->setSpecular(mSpecular.isValid() ? BlueprintEditor::toVec3f(mSpecular) : Ogre::Vector3(1.0f, 1.0f, 1.0f));
        // todo...
    }
    mpDatablock->setRoughness(mRoughness);
    mpDatablock->setEmissive(mEmissive.isValid() ? BlueprintEditor::toVec3f(mEmissive) : Ogre::Vector3(1.0f, 1.0f, 1.0f));
    mpDatablock->setFresnel(mFresnel.isValid() ? BlueprintEditor::toVec3f(mFresnel) : Ogre::Vector3(1.0f, 1.0f, 1.0f), true);

    for (const PbsCommonTextureCard::Info& item : mTextureInfoSet)
    {
        if (item.texture.empty())
            continue;

        mpDatablock->setTexture(item.type, item.texture, &item.sampler);
        if (item.uv > -1) {
            mpDatablock->setTextureUvSource(item.type, item.uv);
        }
    }

    for (const PbsDetailTextureCard::Info item : mDetailInfoSet)
    {
        if (item.texture.empty() || item.indexOffset < 0)
            continue;

        mpDatablock->setTexture(item.type, item.texture, &item.sampler);
        if (item.uv > -1) {
            mpDatablock->setTextureUvSource(item.type, item.uv);
        }

        mpDatablock->setDetailMapBlendMode(item.indexOffset, item.blendMode);
        if (item.offsetScaleEnable) {
            mpDatablock->setDetailMapOffsetScale(item.indexOffset, { item.offsetScale[0], item.offsetScale[1], item.offsetScale[2], item.offsetScale[3] });
        }
    }

    if (mTransparentInfo.enable) {
        mpDatablock->setTransparency(mTransparentInfo.transparency, mTransparentInfo.mode, mTransparentInfo.alphaFromTex);
    }
    else {
        mpDatablock->setTransparency(1.0f, Ogre::HlmsPbsDatablock::None, true);
    }

    if (notify) {
        mpOutputCell->valueChanged(QString(mstrName.c_str()));
    }
}

void PbsDatablockCard::getHitSubItems(const Ogre::IdString& id, Ogre::SceneNode* node, std::set<Ogre::SubItem*>& items)
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
                    subItem->setDatablock(mpPbs->getDefaultDatablock());
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
