#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/ParticleBillboardSettingCard.h"
#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKVectorEditor.h"

#define UpdateFunction(x) [this](BKUnit* unit, const QVariant& data) -> bool { \
x; \
refreshParam();\
return true;\
}

QVariant ParticleBillboardSettingCard::getCurrentCardValue()
{
    return QVariant::fromValue(mData);
}

ParticleBillboardSettingCard::ParticleBillboardSettingCard()
{
	setTitle("广告板设置");

    BKLabel* outputLabel = BKCreator::create<BKLabel>()
        ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
        ->setText("输出");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output)
        ->setDataType(BKAnchor::Output, QMetaTypeId<Data>::qt_metatype_id())
        ->append(outputLabel, false);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

	_pack({
		outputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(
			BKCreator::create<BKLabel>()->setText("公告板类型")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKComboBox>()
                ->setItems(particle::billboardName2Type.keys())
				->setCallbackParamType(BKComboBox::CallbackParamType::Data)
				->setCurrentItem(mData.type)
                ->setDataChangeCallback(UpdateFunction(mData.type = data.toString()))
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("公告板起始点")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKComboBox>()
                ->setItems(particle::billboardName2Origin.keys())
                ->setCallbackParamType(BKComboBox::CallbackParamType::Data)
                ->setCurrentItem(mData.origin)
                ->setDataChangeCallback(UpdateFunction(mData.origin = data.toString()))
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("公告板旋转类型")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKComboBox>()
                ->setItems(particle::billboardName2Rotation.keys())
                ->setCallbackParamType(BKComboBox::CallbackParamType::Data)
                ->setCurrentItem(mData.rotationType)
                ->setDataChangeCallback(UpdateFunction(mData.rotationType = data.toString()))
        ),

		BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("通用向上")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
				->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.commonUp[0], mData.commonUp[1] ,mData.commonUp[2]}))
                ->setItemInLine(3)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
					BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
					mData.commonUp = { vec[0], vec[1], vec[2] };
					refreshParam();
					return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("通用方向")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
                ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.commonDirection[0], mData.commonDirection[1] ,mData.commonDirection[2]}))
                ->setItemInLine(3)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
                    mData.commonDirection = { vec[0], vec[1], vec[2] };
                    refreshParam();
                    return true;
                })
        )
		});
}

void ParticleBillboardSettingCard::refreshParam()
{
    mpOutputAnchor->dataChanged(getCurrentCardValue());
}
#endif