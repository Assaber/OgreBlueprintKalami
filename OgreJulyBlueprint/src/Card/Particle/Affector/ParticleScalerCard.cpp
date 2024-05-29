#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleScalerCard.h"
#include "unit/BKLabel.h"
#include "unit/BKSliderBar.h"

ParticleScalerCard::ParticleScalerCard()
    : mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleScalerCard::Type), &mData })
{
    setTitle("缩放影响器");


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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("每秒在xy方向的缩放")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(100.0f)          // 这里的范围应该是什么...
                ->setCurrentValue(mData.scale)
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    mData.scale = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
        });
}

QVariant ParticleScalerCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}
#endif