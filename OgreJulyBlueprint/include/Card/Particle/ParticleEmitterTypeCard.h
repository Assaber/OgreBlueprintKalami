#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "ParticleBasic.h"

class ParticleEmitterTypeCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleEmitterTypeCard)
public:
	struct Data
	{
		QString type = particle::emitterName2Type.key(particle::PEmitterType::Point);
		int width = 100;
		int height = 100;
		int depth = 100;
		int innerWidth = 50;
		int innerHeight = 50;
		int innerDepth = 50;
	};

public:
	ParticleEmitterTypeCard();
	virtual QVariant getCurrentCardValue() override;

private:
	bool slotEmitterTypeChanged(BKUnit* unit, const QVariant& var);
	bool slotWidthChanged(BKUnit* unit, const QVariant& var);
	bool slotHeightChanged(BKUnit* unit, const QVariant& var);
	bool slotDepthChanged(BKUnit* unit, const QVariant& var);
	bool slotInnerWidthChanged(BKUnit* unit, const QVariant& var);
	bool slotInnerHeightChanged(BKUnit* unit, const QVariant& var);
	bool slotInnerDepthChanged(BKUnit* unit, const QVariant& var);

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
	Data mData;
};
Q_DECLARE_METATYPE(ParticleEmitterTypeCard::Data)
#endif