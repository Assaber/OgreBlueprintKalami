#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleColourInterpolatorCard.h"
#include "unit/BKLabel.h"
#include "unit/BKColorSelector.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSliderBar.h"

ParticleColourInterpolatorCard::ParticleColourInterpolatorCard()
	: mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleColourInterpolatorCard::Type), &mData })
{
    setTitle("颜色插值影响器");

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

		BKCreator::create(BKAnchor::AnchorType::None)->append({
				BKCreator::create<BKLabel>("使能")->setMinWidth(BKCheckBox::fixedWidth),
				BKCreator::create<BKLabel>("颜色")->setMinWidth(BKColorSelector::fixedWidth),
				BKCreator::create<BKLabel>("时间")->setMinWidth(70)
			}),

		 BKCreator::create(BKAnchor::AnchorType::None, BKCell::Type::ListGroup)
			->setTemplate({
				BKCreator::create<BKCheckBox>(),
				BKCreator::create<BKColorSelector>(BKColorSelector::Type::Vector4)->setColor(Qt::white),
                BKCreator::create<BKSliderBar>()
					->setMinimum(0)
					->setMaximum(20.0f)
					->setCurrentValue(1.0f)
					->setMinWidth(70)
			 })
			->setMemberMaximum(mnFixedMaxGroup)
			->setMemberCountChangedCallback(GROUPED_CELL_INNER_FUNC_BIND(ParticleColourInterpolatorCard::slotGroupChanged))
			->setMemberDataChangedCallback(GROUPED_CELL_INNER_FUNC_BIND(ParticleColourInterpolatorCard::slotGroupChanged))
		});
}


void ParticleColourInterpolatorCard::slotGroupChanged(size_t count, const QVariantList& params)
{
	//todo...
	mData.items.clear();
	mData.items.reserve(count / 3);
    for (QVariantList::const_iterator itor = params.begin(); itor != params.end(); )
    {
		Data::Item object;

		object.enable = itor->toBool();
        ++itor;
		object.color = itor->value<QColor>();
        ++itor;
        object.time = itor->toDouble();
        ++itor;

		mData.items.push_back(object);
    }

	mpOutputAnchor->dataChanged(getCurrentCardValue());
}

QVariant ParticleColourInterpolatorCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}

#endif