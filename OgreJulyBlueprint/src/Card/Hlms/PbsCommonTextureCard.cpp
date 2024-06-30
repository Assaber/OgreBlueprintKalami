#include "Card/Hlms/PbsCommonTextureCard.h"

#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKPixmap.h"
#include "unit/BKColorSelectorEx.h"
#include "BlueprintEditor.h"

#include "OgreResourceManager.h"

#include <QFileInfo>

const QMap<QString, Ogre::TextureAddressingMode> name2TextureAddrMode = {
    { "Unknow", Ogre::TAM_UNKNOWN },
    { "Wrap", Ogre::TAM_WRAP },
    { "Mirror", Ogre::TAM_MIRROR },
    { "Clamp", Ogre::TAM_CLAMP },
    { "Border", Ogre::TAM_BORDER },
};

const QMap<QString, Ogre::PbsTextureTypes> name2TextureType = {
    {"diffuse_map", Ogre::PbsTextureTypes::PBSM_DIFFUSE },
    {"normal_map", Ogre::PbsTextureTypes::PBSM_NORMAL },
    {"specular_map", Ogre::PbsTextureTypes::PBSM_SPECULAR },
    {"roughness_map", Ogre::PbsTextureTypes::PBSM_ROUGHNESS },
    {"emissive_map", Ogre::PbsTextureTypes::PBSM_EMISSIVE },
    {"detail_weight_map", Ogre::PbsTextureTypes::PBSM_DETAIL_WEIGHT },
    {"reflection_map", Ogre::PbsTextureTypes::PBSM_REFLECTION },
};

PbsCommonTextureCard::PbsCommonTextureCard()
    : ResetResourceSup("ResourceGroup.Pbs.Snow")
{
    setTitle("Pbs texture");

    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, QMetaTypeId<PbsCommonTextureCard::Info>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("Output")
            ->setMinWidth(140)
        , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    QStringList textureTypeNames = name2TextureType.keys();
    int initTextureTypeIndex = textureTypeNames.indexOf(name2TextureType.key(mTextureInfo.type));

    QStringList textureAddrMode = name2TextureAddrMode.keys();
    int initTextureAddrModeIndex = textureAddrMode.indexOf(name2TextureAddrMode.key(Ogre::TAM_WRAP));

    _pack({
        outputCell,

         BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Type")),
         BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(textureTypeNames)
            ->setCurrentIndex(initTextureTypeIndex, false)
            ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                mTextureInfo.type = name2TextureType[param.toString()];
                outputCell->valueChanged(mTextureInfo);
                return true;
                })
         ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Pixmap")),
        BKCreator::create(BKAnchor::AnchorType::Input)
            ->setDataType(BKAnchor::Input, BKAnchor::String)
            ->append(BKCreator::create<BKPixmap>()
                ->setFixedSize({100, 100})
                ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                    QString qparam = param.toString();
                    Ogre::String texturePath = qparam.toStdString();
                    resetResourceDir(texturePath);

                    mTextureInfo.texture = QFileInfo(param.toString()).fileName().toStdString();
                    outputCell->valueChanged(mTextureInfo);
                    return true;
                    })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Sampler")),
        BKCreator::create(BKAnchor::AnchorType::None)
                ->append({
                    BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("U"),
                    BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("V"),
                    BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("W"),
                }),
        BKCreator::create(BKAnchor::AnchorType::None)
                ->append({
                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(textureAddrMode)
                    ->setCurrentIndex(initTextureAddrModeIndex, false)
                    ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                        mTextureInfo.sampler.mU = name2TextureAddrMode[param.toString()];
                        outputCell->valueChanged(mTextureInfo);
                        return true;
                        }),

                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(textureAddrMode)
                    ->setCurrentIndex(initTextureAddrModeIndex, false)
                    ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                        mTextureInfo.sampler.mV = name2TextureAddrMode[param.toString()];
                        outputCell->valueChanged(mTextureInfo);
                        return true;
                        }),

                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(textureAddrMode)
                    ->setCurrentIndex(initTextureAddrModeIndex, false)
                    ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                        mTextureInfo.sampler.mW = name2TextureAddrMode[param.toString()];
                        outputCell->valueChanged(mTextureInfo);
                        return true;
                        }),

                }),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("UV")),
        BKCreator::create(BKAnchor::AnchorType::None)
            ->append(BKCreator::create<BKComboBox>()
                ->setItems(QStringList() << "" << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7")
                ->setCurrentItem(0, false)
                ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                    QString si = param.toString();
                    mTextureInfo.uv = si.isEmpty() ? -1 : si.toInt();
                    outputCell->valueChanged(mTextureInfo);
                    return true;
                    })
            )
        });
}

QVariant PbsCommonTextureCard::getCurrentCardValue()
{
    return mTextureInfo;
}