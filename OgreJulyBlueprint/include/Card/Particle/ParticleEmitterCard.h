#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include <QColor>
#include "container/BKCard.h"
#include "Particle/ParticleEmitterTypeCard.h"
#include <array>

class BKAnchor;
class ParticleEmitterCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleEmitterCard)
public:
	struct Data
	{
		QString name;
		QString nextName;

		ParticleEmitterTypeCard::Data type;

		int32_t degrees = 0;

		std::array<float, 3> position = { 0, 0, 0 };
		std::array<float, 3> direction = { 1, 0, 0 };
		int32_t sendItemPerSec = 10;
		
		QColor color;
		QColor startColor;
		QColor endColor;

		float velocity = 1;
		float velocityMin = 1;
		float velocityMax = 1;

		float timeToLive = 5;
		float timeToLiveMin = 5;
		float timeToLiveMax = 5;

		float duration = 0;
		float durationMin = 0;
		float durationMax = 0;

		float repeatDelay = 0;
		float repeatDelayMin = 0;
		float repeatDelayMax = 0;
	};

public:
	ParticleEmitterCard();
	virtual QVariant getCurrentCardValue() override;

private:
	BKAnchor* mpOutputAnchor = nullptr;

private:
    Data mData;
};
Q_DECLARE_METATYPE(ParticleEmitterCard::Data)
#endif

