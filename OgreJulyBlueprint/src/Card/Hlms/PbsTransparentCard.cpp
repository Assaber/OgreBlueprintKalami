#include "Card/Hlms/PbsTransparentCard.h"

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

PbsTransparentCard::PbsTransparentCard()
{
    setTitle("Pbs transparent");

    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, QMetaTypeId<PbsTransparentCard::Info>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("Output")
            ->setMinWidth(80)
            , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    _pack({
        outputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            {
                BKCreator::create<BKLabel>()->setText("Enable"),
                BKCreator::create<BKCheckBox>()
                    ->setChecked(mInfo.enable)
                    ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                        mInfo.enable = param.toBool();
                        outputCell->valueChanged(mInfo);
                        return true;
                    })
            }),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Transparent")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)
            ->setMinimum(0)
            ->setMaximum(1.0f)
            ->setCurrentValue(mInfo.transparency)
            ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                mInfo.transparency = param.toFloat();
                outputCell->valueChanged(mInfo);
                return true;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Mode")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(QStringList() << "None" << "Transparent" << "Fade")
            ->setCurrentIndex(1)
            ->setCallbackParamType(BKComboBox::CallbackParamType::Index)
            ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {

                switch (param.toInt())
                {
                case 0:
                    mInfo.mode = Ogre::HlmsPbsDatablock::None;
                    break;
                case 1:
                    mInfo.mode = Ogre::HlmsPbsDatablock::Transparent;
                    break;
                case 2:
                    mInfo.mode = Ogre::HlmsPbsDatablock::Fade;
                    break;
                default:
                    break;
                }

                outputCell->valueChanged(mInfo);
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Alpha from texture")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKCheckBox>()
                ->setChecked(mInfo.alphaFromTex)
                ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                    mInfo.alphaFromTex = param.toBool();
                    outputCell->valueChanged(mInfo);
                    return true;
                    })
            ),
        });
}

QVariant PbsTransparentCard::getCurrentCardValue()
{
    return mInfo;
}