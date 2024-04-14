#include "Card/Hlms/PbsMapCard.h"

#include "BKCreator.h"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKPixmap.h"
#include "unit/BKColorSelectorEx.h"
#include "BlueprintEditor.h"

PbsMapCard::PbsMapCard()
{
    setTitle("Pbs贴图");

    mpOutputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    mpOutputCell->setDataType(BKAnchor::Output, QMetaTypeId<PbsMapCard::TexInfo>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("输出")
            ->setMinWidth(140)
        , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    static const QMap<QString, Ogre::PbsTextureTypes> validSupport = {
        {"diffuse_map", Ogre::PbsTextureTypes::PBSM_DIFFUSE },
        {"normal_map", Ogre::PbsTextureTypes::PBSM_NORMAL },
        {"specular_map", Ogre::PbsTextureTypes::PBSM_SPECULAR },
        {"roughness_map", Ogre::PbsTextureTypes::PBSM_ROUGHNESS },
        {"emissive_map", Ogre::PbsTextureTypes::PBSM_EMISSIVE },
        {"detail_weight_map", Ogre::PbsTextureTypes::PBSM_DETAIL_WEIGHT },
        {"reflection_map", Ogre::PbsTextureTypes::PBSM_REFLECTION },
    };

    _pack({
        mpOutputCell,

         BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("贴图类型")),
         BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(validSupport.keys())
            ->setCurrentIndex(0, false)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mTextureInfo.type = validSupport[param.toString()];
                mpOutputCell->valueChanged(mTextureInfo);
                return true;
                })
         ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("贴图")),
        BKCreator::create(BKAnchor::AnchorType::Input)
            ->setDataType(BKAnchor::Input, BKAnchor::String)
            ->append(BKCreator::create<BKPixmap>()
                ->setFixedSize({100, 100})
                ->setDataChangeCallback([this](const QVariant& param) -> bool {
                    mTextureInfo.texture = param.toString().toStdString();
                    mpOutputCell->valueChanged(mTextureInfo);
                    return true;
                    })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("UV")),
        BKCreator::create(BKAnchor::AnchorType::None)
            ->append(BKCreator::create<BKComboBox>()
                ->setItems(QStringList() << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7")
                ->setCurrentItem("", false)
                ->setDataChangeCallback([this](const QVariant& param) -> bool {
                    QString si = param.toString();
                    mTextureInfo.uv = si.toInt();
                    mpOutputCell->valueChanged(mTextureInfo);
                    return true;
                    })
            )
        });
}

QVariant PbsMapCard::getCurrentCardValue()
{
    return mTextureInfo;
}
