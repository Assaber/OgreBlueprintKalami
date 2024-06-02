#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "container/BKCard.h"
#include "OgreString.h"
#include "ParticleBasic.h"

namespace Ogre {
	class ParticleSystem;
    class ParticleEmitter;
	class SceneManager;
}

class BKAnchor;
class ParticleCard : public BKCard
{
	CREATE_CARD_FACTORY(ParticleCard)

public:
	struct Data
	{
		Ogre::String id;
		Ogre::String material;
		int quota = 1;
		int itemWidth = 1;
		int itemHeight = 1;
		Ogre::String renderType = "";
		bool isPointRendering = false;
		bool isAccurateFacing = false;
		bool isCullEach = false;
		bool isSorted = false;
		bool isLocalSpace = false;
		float updateInterval = .0f;
		float updateTimeout = .0f;
	};

public:
	ParticleCard();
	~ParticleCard();

private:
	void refreshParam();
	bool updateRenderType(BKUnit* unit, const QVariant& var);
	bool updateEmitter(BKUnit* unit, const QVariant& var);
	bool updateAffector(BKUnit* unit, const QVariant& var);

	void reloadParticleToScene();

private:
	BKAnchor* mpOutputAnchor = nullptr;
	BKAnchor* mpEmitterAnchor = nullptr;
	BKAnchor* mpAffectorAnchor = nullptr;

	Ogre::ParticleSystem* mpParticleSystemTemplate = nullptr;
	Ogre::ParticleSystem* mpSceneParticleSystem = nullptr;
	Ogre::SceneNode* mpBindNodeParent = nullptr;
	Ogre::SceneNode* mpBindNode = nullptr;
	Ogre::SceneManager* mpSceneManager = nullptr;

private:
	Data mData;
};
#endif