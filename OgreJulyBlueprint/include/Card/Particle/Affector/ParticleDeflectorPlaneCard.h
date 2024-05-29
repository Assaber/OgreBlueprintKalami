#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreDeflectorPlaneAffector.h"
#include <array>

class ParticleDeflectorPlaneCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleDeflectorPlaneCard)
public:
    enum Affector { Type = particle::ParticleAffector::DeflctorPlane};

	struct Data
	{
		std::array<float, 3> point = { .0f, .0f, .0f };
		std::array<float, 3> normal = { .0f, 1.0f, .0f };
		float bounce = 1.0f;
	};

public:
	ParticleDeflectorPlaneCard();
    virtual QVariant getCurrentCardValue() override;

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
    particle::ParticleAffector mAffector;
};
#endif