#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleRotatorCard.h"
#include "unit/BKLabel.h"
#include "unit/BKSliderBar.h"

ParticleRotatorCard::ParticleRotatorCard()
	: mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleRotatorCard::Type), &mData })
{
    setTitle("旋转影响器");

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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("旋转起始速度(°/s)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.speedStart)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.speedStart = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("旋转终止速度(°/s)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.speedEnd)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.speedEnd = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("旋转起始角(°)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.rotStart)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.rotStart = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("旋转起终止角(°)")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(360)
                ->setCurrentValue(mData.rotEnd)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
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