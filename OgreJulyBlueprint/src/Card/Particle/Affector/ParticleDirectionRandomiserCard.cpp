#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleDirectionRandomiserCard.h"
#include "unit/BKLabel.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"

ParticleDirectionRandomiserCard::ParticleDirectionRandomiserCard()
{
    setTitle("Direction randomiser");

    BKLabel* outputLabel = BKCreator::create<BKLabel>()
        ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
        ->setText("Output");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output)
        ->setDataType(BKAnchor::Output, GET_QT_METATYPE_ID(particle::ParticleAffector))
        ->append(outputLabel, false);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

    _pack({
        outputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Randomness")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(1'000.0f)
                ->setCurrentValue(mData.randomness)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.randomness = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Scope")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(1.0f)
                ->setCurrentValue(mData.scope)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.scope = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Keep velocity")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKCheckBox>()
                ->setChecked(mData.keepVelocity)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
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