#pragma once
#include "container/BKCard.h"
#include <OgreString.h>

namespace Ogre{
    class TextureGpu;
    class TextureGpuManager;
}

class CanvasCard : public BKCard
{
    CREATE_CARD_FACTORY(CanvasCard)
public:
    CanvasCard();
    ~CanvasCard();

private:
    bool slotTextureUpdate(BKUnit* unit, const QVariant& image);
    void uploadTextureData(int width, int height, const uint8_t* data);

private:
    Ogre::String mstrTextureUuid = "";
    Ogre::TextureGpuManager* mpTextureManager = nullptr;
    Ogre::TextureGpu* mpTexture = nullptr;
};