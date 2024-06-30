#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/ParticleEmitterCard.h"
#include "unit/BKLabel.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKVectorEditor.h"
#include "unit/BKColorSelectorEx.h"
#include <QUuid>

#define CREATE_INT_SLIDERBAR_WITH_TITLE(title, bind, min, max) \
BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>(title)), \
BKCreator::create(BKAnchor::AnchorType::None)->append( \
    BKCreator::create<BKSliderBar>() \
    ->setMaximum(max) \
    ->setMinimum(min) \
    ->setCurrentValue(bind) \
    ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool { \
        bind = data.toInt(); \
        mpOutputAnchor->dataChanged(getCurrentCardValue()); \
        return true; \
        }) \
)

#define CREATE_DOUBLE_SLIDERBAR_WITH_TITLE(title, bind, min, max) \
BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>(title)), \
BKCreator::create(BKAnchor::AnchorType::None)->append( \
    BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double) \
    ->setMaximum(max) \
    ->setMinimum(min) \
    ->setCurrentValue(bind) \
    ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool { \
        bind = data.toDouble(); \
        mpOutputAnchor->dataChanged(getCurrentCardValue()); \
        return true; \
        }) \
)


QVariant ParticleEmitterCard::getCurrentCardValue()
{
    return QVariant::fromValue(mData);
}

ParticleEmitterCard::ParticleEmitterCard()
{
	setTitle("Emitter");

    BKLabel* outputLabel = BKCreator::create<BKLabel>()
        ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
        ->setText("Output");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output)
        ->setDataType(BKAnchor::Output, QMetaTypeId<Data>::qt_metatype_id())
        ->append(outputLabel, false);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

	mData.name = QUuid::createUuid().toString(QUuid::WithoutBraces);

	_pack({ 
		outputCell,

		BKCreator::create(BKAnchor::AnchorType::Input)
            ->setDataType(BKAnchor::Input, QMetaTypeId<ParticleEmitterTypeCard::Data>::qt_metatype_id())
            ->append(
                BKCreator::create<BKLabel>("Type")
                ->setTitleNeverChanges(true)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.type = data.value<ParticleEmitterTypeCard::Data>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Name")),
		BKCreator::create(BKAnchor::AnchorType::None)->append(
			BKCreator::create<BKLineEdit>()
				->setText(mData.name)
				->setEnable(false)
		),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Next emitter")),
        BKCreator::create(BKAnchor::AnchorType::Input)
			->setDataType(BKAnchor::Input, BKAnchor::String)
			->append(BKCreator::create<BKLineEdit>()
                ->setText(mData.nextName)
                ->setEnable(false)
				->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.nextName = data.toString();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Offset degress range")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()
			->setMaximum(180)
			->setMinimum(0)
			->setCurrentValue(mData.degrees)
			->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
					mData.degrees = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
					return true;
				})
		),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Position")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
			->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.position[0], mData.position[1], mData.position[2]}))
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
					BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
					mData.position = { vec[0], vec[1], vec[2] };
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Direction")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
            ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.direction[0], mData.direction[1], mData.direction[2]}))
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
                    mData.direction = { vec[0], vec[1], vec[2] };
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

		CREATE_INT_SLIDERBAR_WITH_TITLE("Send count per second", mData.sendItemPerSec, 1, 5'000),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Color")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>()
            ->setColor(mData.color)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.color = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Start color")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>()
            ->setColor(mData.startColor)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.startColor = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("End color")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>()
            ->setColor(mData.endColor)
            ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.endColor = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Velocity", mData.velocity, 1.0f, 1'000.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Min velocity", mData.velocityMin, 1.0f, 1'000.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Max velocity", mData.velocityMax, 1.0f, 1'000.0f),


        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Time to live", mData.timeToLive, 0, 60.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Min time to live", mData.timeToLiveMin, 0, 60.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Max time to live", mData.timeToLiveMax, 0, 60.0f),

        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Duration", mData.duration, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Min duration", mData.durationMin, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Max duration", mData.durationMax, 0, 20.0f),

        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Repeat delay", mData.repeatDelay, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Min repeat delay", mData.repeatDelayMin, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("Max repeat delay", mData.repeatDelayMax, 0, 20.0f),

		});
}
#endif