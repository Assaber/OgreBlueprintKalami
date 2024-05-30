#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleDirectionRandomiserCard.h"
#include "unit/BKLabel.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"

ParticleDirectionRandomiserCard::ParticleDirectionRandomiserCard()
{
    setTitle("方向随机影响器");

    BKLabel* outputLabel = BKCreator::create<BKLabel>()
        ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
        ->setText("输出");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output)
        ->setDataType(BKAnchor::Output, GET_QT_METATYPE_ID(particle::ParticleAffector))
        ->append(outputLabel, false);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

    _pack({
        outputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("随机性大小")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(1'000.0f)
                ->setCurrentValue(mData.randomness)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.randomness = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("受影响粒子百分比")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(1.0f)
                ->setCurrentValue(mData.scope)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.scope = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("是否保持粒子速度")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKCheckBox>()
                ->setChecked(mData.keepVelocity)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.keepVelocity = data.toBool();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
    });
}

QVariant ParticleDirectionRandomiserCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}
#endif