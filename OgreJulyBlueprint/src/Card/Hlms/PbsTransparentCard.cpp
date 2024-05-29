#include "Card/Hlms/PbsTransparentCard.h"

#include "BKCreator.h"
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

#include <QMessageBox>
#include <QUuid>

PbsTransparentCard::PbsTransparentCard()
{
    setTitle("Pbs透明");

    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, QMetaTypeId<PbsTransparentCard::Info>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("输出")
            ->setMinWidth(80)
            , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    _pack({
        outputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            {
                BKCreator::create<BKLabel>()->setText("使能"),
                BKCreator::create<BKCheckBox>()
                    ->setEnable(mInfo.enable)
                    ->setDataChangeCallback([this, outputCell](const QVariant& param) -> bool {
                        mInfo.enable = param.toBool();
                        outputCell->valueChanged(mInfo);
                        return true;
                    })
            }),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("透明度")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)
            ->setMinimum(0)
            ->setMaximum(1.0f)
            ->setCurrentValue(mInfo.transparency)
            ->setDataChangeCallback([this, outputCell](const QVariant& param) -> bool {
                mInfo.transparency = param.toFloat();
                outputCell->valueChanged(mInfo);
                return true;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("透明模式")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(QStringList() << "None" << "Transparent" << "Fade")
            ->setCurrentIndex(1)
            ->setCallbackParamType(BKComboBox::CallbackParamType::Index)
            ->setDataChangeCallback([this, outputCell](const QVariant& param) -> bool {
                int mode = param.toInt();
                if (mode == 0)
                    mInfo.mode = Ogre::HlmsPbsDatablock::None;
                else if (mode == 1)
                    mInfo.mode = Ogre::HlmsPbsDatablock::Transparent;
                else if (mode == 2)
                    mInfo.mode = Ogre::HlmsPbsDatablock::Fade;
                outputCell->valueChanged(mInfo);
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("从纹理获取透明值")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKCheckBox>()
                ->setEnable(mInfo.alphaFromTex)
                ->setDataChangeCallback([this, outputCell](const QVariant& param) -> bool {
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