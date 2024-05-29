#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreRotationAffector.h"

class ParticleRotatorCard : public BKCard
{
    CREATE_CARD_FACTORY(ParticleRotatorCard)
public:
    enum Affector { Type = particle::ParticleAffector::Rotator };
	struct Data
	{
        int speedStart = 0;
        int speedEnd = 0;
        int rotStart = 0;
        int rotEnd = 0;
	};

public:
	ParticleRotatorCard();
    virtual QVariant getCurrentCardValue() override;

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
    particle::ParticleAffector mAffector;
};

#endif