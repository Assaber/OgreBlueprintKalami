#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreColourFaderAffector.h"

class ParticleColourFaderCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleColourFaderCard)
public:
	enum Affector { Type = particle::ParticleAffector::ColourFader };
	struct Data
	{
		QColor color;
	};

public:
	ParticleColourFaderCard();
    virtual QVariant getCurrentCardValue() override;

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
    particle::ParticleAffector mAffector;
};

#endif