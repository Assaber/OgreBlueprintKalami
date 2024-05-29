#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreScaleAffector.h"

class ParticleScalerCard : public BKCard
{
    CREATE_CARD_FACTORY(ParticleScalerCard)

public:
    enum Affector { Type = particle::ParticleAffector::Scaler };
	struct Data
	{
		float scale = 1.0f;
	};

public:
    ParticleScalerCard();
    virtual QVariant getCurrentCardValue() override;

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
    particle::ParticleAffector mAffector;
};
#endif