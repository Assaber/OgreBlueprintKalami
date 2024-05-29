#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "ParticleBasic.h"
#include <array>

class BKAnchor;
class ParticleBillboardSettingCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleBillboardSettingCard)
public:
	struct Data
	{
        QString type = particle::billboardName2Type.key(Ogre::v1::BBT_POINT);
        QString origin = particle::billboardName2Origin.key(Ogre::v1::BBO_CENTER);
        QString rotationType = particle::billboardName2Rotation.key(Ogre::v1::BBR_TEXCOORD);
		std::array<float, 3> commonUp = {0, 0, 1};
		std::array<float, 3> commonDirection = { 0, 1, 0 };
	};

	virtual QVariant getCurrentCardValue() override;

public:
	ParticleBillboardSettingCard();

public:
	void refreshParam();

private:
    BKAnchor* mpOutputAnchor = nullptr;

private:
	Data mData;
};
Q_DECLARE_METATYPE(ParticleBillboardSettingCard::Data)
#endif

