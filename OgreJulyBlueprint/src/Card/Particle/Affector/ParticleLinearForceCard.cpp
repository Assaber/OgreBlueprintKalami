#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleLinearForceCard.h"
#include "unit/BKLabel.h"
#include "unit/BKVectorEditor.h"
#include "unit/BKComboBox.h"

ParticleLinearForceCard::ParticleLinearForceCard()
    : mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleLinearForceCard::Type), &mData })
{
    setTitle("线性力影响器");

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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("力")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
                ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.force[0], mData.force[1] ,mData.force[2]}))
                ->setItemInLine(3)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
                    mData.force = { vec[0], vec[1], vec[2] };
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("模式")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKComboBox>()
                ->setItems(particle::linearAffectorName2Type.keys())
                ->setCallbackParamType(BKComboBox::CallbackParamType::Data)
                ->setCurrentItem(mData.function)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.function = data.toString();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
        });
}

QVariant ParticleLinearForceCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}

#endif