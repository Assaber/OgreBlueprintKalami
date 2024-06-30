#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleColourFaderCard.h"
#include "unit/BKLabel.h"
#include "unit/BKColorSelectorEx.h"
#include "unit/BKSliderBar.h"

ParticleColourFaderCard::ParticleColourFaderCard()
    : mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleColourFaderCard::Type), &mData })
{
    setTitle("Colour fader");

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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Color")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>(BKColorSelectorEx::Type::Vector4)
                ->setColor(mData.color)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.color = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                    })
        )
        });
}

QVariant ParticleColourFaderCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}

#endif