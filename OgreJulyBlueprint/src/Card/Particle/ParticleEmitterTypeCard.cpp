#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/ParticleEmitterTypeCard.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKSliderBar.h"

ParticleEmitterTypeCard::ParticleEmitterTypeCard()
{
	setTitle("Emitter type");
	
	BKLabel* outputLabel = BKCreator::create<BKLabel>();
	outputLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	outputLabel->setText("Output");
	outputLabel->setMinWidth(140);
	BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
	outputCell->setDataType(BKAnchor::Output, QMetaTypeId<Data>::qt_metatype_id());
	outputCell->append(outputLabel, false);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

    BKLabel* typeLabel = BKCreator::create<BKLabel>();
	typeLabel->setText("Render type");
    BKCell* typeLabelCell = BKCreator::create(BKAnchor::AnchorType::None);
	typeLabelCell->append(typeLabel);
    BKComboBox* typeComboBox = BKCreator::create<BKComboBox>();
	typeComboBox->setItems(particle::emitterName2Type.keys());
	typeComboBox->setCurrentItem(mData.type);
	typeComboBox->setDataChangeCallback(std::bind(&ParticleEmitterTypeCard::slotEmitterTypeChanged, this, std::placeholders::_1, std::placeholders::_2));
    BKCell* typeComboBoxCell = BKCreator::create(BKAnchor::AnchorType::None);
	typeComboBoxCell->append(typeComboBox);

	auto create_slider = [](const QString& name, int value, BKUnit::DataChangeCallback bind) -> std::pair<BKCell*, BKCell*> {
        BKLabel* label = BKCreator::create<BKLabel>();
		label->setText(name);
        BKCell* lc = BKCreator::create(BKAnchor::AnchorType::None);
		lc->append(label);

        BKSliderBar* slider = BKCreator::create<BKSliderBar>();
        slider->setMinimum(0);
        slider->setMaximum(1'000);
        slider->setCurrentValue(value);
        slider->setDataChangeCallback(bind);
        BKCell* sc = BKCreator::create(BKAnchor::AnchorType::None);
		sc->append(slider);

		return { lc, sc };
	};

	auto widthCellPair = create_slider("Width", mData.width, std::bind(&ParticleEmitterTypeCard::slotWidthChanged, this, std::placeholders::_1, std::placeholders::_2));
	auto heightCellPair = create_slider("Height", mData.height, std::bind(&ParticleEmitterTypeCard::slotHeightChanged, this, std::placeholders::_1, std::placeholders::_2));
	auto depthCellPair = create_slider("Depth", mData.depth, std::bind(&ParticleEmitterTypeCard::slotDepthChanged, this, std::placeholders::_1, std::placeholders::_2));
    auto innerWidthCellPair = create_slider("Inner width", mData.innerWidth, std::bind(&ParticleEmitterTypeCard::slotInnerWidthChanged, this, std::placeholders::_1, std::placeholders::_2));
    auto innerHeightCellPair = create_slider("Inner height", mData.innerHeight, std::bind(&ParticleEmitterTypeCard::slotInnerHeightChanged, this, std::placeholders::_1, std::placeholders::_2));
    auto innerDepthCellPair = create_slider("Inner depth", mData.innerDepth, std::bind(&ParticleEmitterTypeCard::slotInnerDepthChanged, this, std::placeholders::_1, std::placeholders::_2));

	_pack({
		outputCell,
		typeComboBoxCell,
		widthCellPair.first,
		widthCellPair.second,
		heightCellPair.first,
		heightCellPair.second,
		depthCellPair.first,
		depthCellPair.second,
		innerWidthCellPair.first,
		innerWidthCellPair.second,
		innerHeightCellPair.first,
		innerHeightCellPair.second,
		innerDepthCellPair.first,
		innerDepthCellPair.second
		});
}

QVariant ParticleEmitterTypeCard::getCurrentCardValue()
{
    return QVariant::fromValue(mData);
}

bool ParticleEmitterTypeCard::slotEmitterTypeChanged(BKUnit* unit, const QVariant& var)
{
	mData.type = var.toString();
	mpOutputAnchor->dataChanged(getCurrentCardValue());
	return true;
}

bool ParticleEmitterTypeCard::slotWidthChanged(BKUnit* unit, const QVariant& var)
{
	mData.width = var.toInt();
	mpOutputAnchor->dataChanged(getCurrentCardValue());
	return true;
}

bool ParticleEmitterTypeCard::slotHeightChanged(BKUnit* unit, const QVariant& var)
{
    mData.height = var.toInt();
	mpOutputAnchor->dataChanged(getCurrentCardValue());
    return true;
}

bool ParticleEmitterTypeCard::slotDepthChanged(BKUnit* unit, const QVariant& var)
{
    mData.depth = var.toInt();
	mpOutputAnchor->dataChanged(getCurrentCardValue());
    return true;
}

bool ParticleEmitterTypeCard::slotInnerWidthChanged(BKUnit* unit, const QVariant& var)
{
    mData.innerWidth = var.toInt();
	mpOutputAnchor->dataChanged(getCurrentCardValue());
    return true;
}

bool ParticleEmitterTypeCard::slotInnerHeightChanged(BKUnit* unit, const QVariant& var)
{
    mData.innerHeight = var.toInt();
	mpOutputAnchor->dataChanged(getCurrentCardValue());
    return true;
}

bool ParticleEmitterTypeCard::slotInnerDepthChanged(BKUnit* unit, const QVariant& var)
{
    mData.innerDepth = var.toInt();
	mpOutputAnchor->dataChanged(getCurrentCardValue());
    return true;
}

#endif