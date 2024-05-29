#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreDirectionRandomiserAffector.h"

class ParticleDirectionRandomiserCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleDirectionRandomiserCard)
public:
    enum Affector { Type = particle::ParticleAffector::DirectionRandomiser };
	struct Data
	{
		float randomness = 1.0f;
		float scope = 1.0f;
		bool keepVelocity = false;
	};

public:
	ParticleDirectionRandomiserCard();
	virtual QVariant getCurrentCardValue() override;

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
    particle::ParticleAffector mAffector;
};
#endif