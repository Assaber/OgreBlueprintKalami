#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleColourImageCard.h"
#include "unit/BKLabel.h"
#include "unit/BKLineEdit.h"

ParticleColourImageCard::ParticleColourImageCard()
    : mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleColourImageCard::Type), &mData })
{
    setTitle("图片影响器");

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

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("图片名称")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLineEdit>()
                ->setText(mData.image)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.image = data.toString();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
        });
}

QVariant ParticleColourImageCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}

#endif