#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleColourFader2Card.h"
#include "unit/BKLabel.h"
#include "unit/BKColorSelectorEx.h"
#include "unit/BKSliderBar.h"

ParticleColourFader2Card::ParticleColourFader2Card()
	: mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleColourFader2Card::Type), &mData })
{
	setTitle("颜色渐变 v2");

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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("颜色1")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>(BKColorSelectorEx::Type::Vector4)
                ->setColor(mData.color1)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
				    mData.color1 = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                    })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("颜色2")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>(BKColorSelectorEx::Type::Vector4)
                ->setColor(mData.color2)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.color2 = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                    })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("时间间隔")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(20.0f)
                ->setCurrentValue(mData.time)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
				    mData.time = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
	});
}

QVariant ParticleColourFader2Card::getCurrentCardValue()
{
	return QVariant::fromValue(mAffector);
}

#endif