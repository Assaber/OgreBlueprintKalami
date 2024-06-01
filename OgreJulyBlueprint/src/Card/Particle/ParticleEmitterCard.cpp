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
    ->setDataChangeCallback([this](const QVariant& data) -> bool { \
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
    ->setDataChangeCallback([this](const QVariant& data) -> bool { \
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
	setTitle("粒子发射器");

    BKLabel* outputLabel = BKCreator::create<BKLabel>()
        ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
        ->setText("输出");
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
                BKCreator::create<BKLabel>("发射器类型")
                ->setTitleNeverChanges(true)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.type = data.value<ParticleEmitterTypeCard::Data>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("名称")),
		BKCreator::create(BKAnchor::AnchorType::None)->append(
			BKCreator::create<BKLineEdit>()
				->setText(mData.name)
				->setEnable(false)
		),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("接力发射器名称")),
        BKCreator::create(BKAnchor::AnchorType::Input)
			->setDataType(BKAnchor::Input, BKAnchor::String)
			->append(BKCreator::create<BKLineEdit>()
                ->setText(mData.nextName)
                ->setEnable(false)
				->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.nextName = data.toString();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("允许偏移角度")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()
			->setMaximum(180)
			->setMinimum(0)
			->setCurrentValue(mData.degrees)
			->setDataChangeCallback([this](const QVariant& data) -> bool {
					mData.degrees = data.toInt();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
					return true;
				})
		),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("发射位置")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
			->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.position[0], mData.position[1], mData.position[2]}))
            ->setDataChangeCallback([this](const QVariant& data) -> bool {
					BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
					mData.position = { vec[0], vec[1], vec[2] };
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("发射方向")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
            ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.direction[0], mData.direction[1], mData.direction[2]}))
            ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
                    mData.direction = { vec[0], vec[1], vec[2] };
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

		CREATE_INT_SLIDERBAR_WITH_TITLE("每秒发射个数", mData.sendItemPerSec, 1, 5'000),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("静态颜色")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>()
            ->setColor(mData.color)
            ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.color = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("动态起始颜色")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>()
            ->setColor(mData.startColor)
            ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.startColor = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("动态终止颜色")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKColorSelectorEx>()
            ->setColor(mData.endColor)
            ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.endColor = data.value<QColor>();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("速率", mData.velocity, 1.0f, 1'000.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最小速率", mData.velocityMin, 1.0f, 1'000.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最大速率", mData.velocityMax, 1.0f, 1'000.0f),


        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("存活时间", mData.timeToLive, 0, 60.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最小存活时间", mData.timeToLiveMin, 0, 60.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最大存活时间", mData.timeToLiveMax, 0, 60.0f),

        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("活动状态", mData.duration, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最小活动状态", mData.durationMin, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最大活动状态", mData.durationMax, 0, 20.0f),

        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("重启时间", mData.repeatDelay, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最小重启时间", mData.repeatDelayMin, 0, 20.0f),
        CREATE_DOUBLE_SLIDERBAR_WITH_TITLE("最大重启时间", mData.repeatDelayMax, 0, 20.0f),

		});
}
#endif