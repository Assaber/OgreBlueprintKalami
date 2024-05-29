#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "../ParticleBasic.h"
#include "OgreColourInterpolatorAffector.h"

class ParticleColourInterpolatorCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleColourInterpolatorCard)
public:
    enum Affector { Type = particle::ParticleAffector::ColourInterpolator };
	struct Data
	{
		struct Item
		{
			bool enable = false;
			QColor color;
			float time = .0f;
		};

		std::vector<Item> items;
	};

public:
	ParticleColourInterpolatorCard();
    virtual QVariant getCurrentCardValue() override;

private:
	void slotGroupChanged(size_t count, const QVariantList& params);

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
	static constexpr int mnFixedMaxGroup = 6;
    Data mData;
    particle::ParticleAffector mAffector;
};
#endif