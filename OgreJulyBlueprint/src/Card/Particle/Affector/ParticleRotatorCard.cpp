#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleRotatorCard.h"
#include "unit/BKLabel.h"
#include "unit/BKSliderBar.h"

ParticleRotatorCard::ParticleRotatorCard()
	: mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleRotatorCard::Type), &mData })
{
    setTitle("Rotator");

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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Speed begin(°/s)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.speedStart)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.speedStart = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Speed end(°/s)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.speedEnd)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.speedEnd = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Rotate begin(°)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.rotStart)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.rotStart = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Rotate end(°)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.rotEnd)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.rotEnd = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
	});
}

QVariant ParticleRotatorCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}

#endif