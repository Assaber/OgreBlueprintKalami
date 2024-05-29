#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreColourImageAffector.h"

class ParticleColourImageCard : public BKCard
{
    CREATE_CARD_FACTORY(ParticleColourImageCard)
public:
    enum Affector { Type = particle::ParticleAffector::ColourImage };
    struct Data
    {
        QString image;
    };

public:
    ParticleColourImageCard();
    virtual QVariant getCurrentCardValue() override;

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
    particle::ParticleAffector mAffector;
};
#endif

