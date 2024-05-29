#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreLinearForceAffector.h"
#include <array>

class ParticleLinearForceCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleLinearForceCard)
public:
	enum Affector { Type = particle::ParticleAffector::LinearForce };
	struct Data
	{
		std::array<float, 3> force = {1.0f, 0, 0};
		QString function = particle::linearAffectorName2Type.key(Ogre::LinearForceAffector::FA_AVERAGE);
	};

public:
	ParticleLinearForceCard();
    virtual QVariant getCurrentCardValue() override;

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
    particle::ParticleAffector mAffector;
};

#endif