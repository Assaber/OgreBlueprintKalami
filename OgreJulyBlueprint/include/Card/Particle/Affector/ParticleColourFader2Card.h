#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreColourFaderAffector2.h"

class ParticleColourFader2Card : public BKCard
{
	CREATE_CARD_FACTORY(ParticleColourFader2Card)
public:
	enum Affector{ Type = particle::ParticleAffector::ColourFader2 };
	struct Data
	{
		float time = 0;
		QColor color1;
		QColor color2;
	};

public:
	ParticleColourFader2Card();
	virtual QVariant getCurrentCardValue() override;

private:
	BKAnchor* mpOutputAnchor = nullptr;

private:
	Data mData;
	particle::ParticleAffector mAffector;
};
#endif

