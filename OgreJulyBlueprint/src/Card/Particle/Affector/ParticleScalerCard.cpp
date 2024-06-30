#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleScalerCard.h"
#include "unit/BKLabel.h"
#include "unit/BKSliderBar.h"

ParticleScalerCard::ParticleScalerCard()
    : mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleScalerCard::Type), &mData })
{
    setTitle("Scaler");


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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Scale rate")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(100.0f)          // What should be the scope here...
                ->setCurrentValue(mData.scale)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.scale = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
        });
}

QVariant ParticleScalerCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}
#endif