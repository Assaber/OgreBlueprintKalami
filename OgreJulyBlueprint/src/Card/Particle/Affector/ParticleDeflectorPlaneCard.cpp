#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/Affector/ParticleDeflectorPlaneCard.h"
#include "unit/BKLabel.h"
#include "unit/BKColorSelectorEx.h"
#include "unit/BKSliderBar.h"
#include "unit/BKVectorEditor.h"

ParticleDeflectorPlaneCard::ParticleDeflectorPlaneCard()
    : mAffector({ static_cast<particle::ParticleAffector::Type>(ParticleDeflectorPlaneCard::Type), &mData })
{
    setTitle("Deflector plane");

    BKLabel* outputLabel = BKCreator::create<BKLabel>()
        ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
        ->setText("Output");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output)
        ->setDataType(BKAnchor::Output, GET_QT_METATYPE_ID(particle::ParticleAffector))
        ->append(outputLabel, false);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

    _pack({
        outputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Point")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
                ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.point[0], mData.point[1] ,mData.point[2]}))
                ->setItemInLine(3)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
                    mData.point = { vec[0], vec[1], vec[2] };
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Normal")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKVectorEditor>()
                ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{mData.normal[0], mData.normal[1] ,mData.normal[2]}))
                ->setItemInLine(3)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    BKVectorEditor::FloatVec vec = data.value<BKVectorEditor::FloatVec>();
                    mData.normal = { vec[0], vec[1], vec[2] };
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>("Bounce")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(1.0f)
                ->setCurrentValue(mData.bounce)
                ->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) -> bool {
                    mData.bounce = data.toDouble();
                    mpOutputAnchor->dataChanged(getCurrentCardValue());
                    return true;
                })
        )
        });
}

QVariant ParticleDeflectorPlaneCard::getCurrentCardValue()
{
    return QVariant::fromValue(mAffector);
}

#endif