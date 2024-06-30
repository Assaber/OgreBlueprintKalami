#pragma once
#include "container/BKCard.h"
#include "OgreHlmsDatablock.h"
#include <array>

class BKCell;
class BKCheckBox;
class BlendblockCard : public BKCard
{
    CREATE_CARD_FACTORY(BlendblockCard)
public:
    BlendblockCard();
    virtual QVariant getCurrentCardValue() override;

private:
    void updateBlendChannelMask(BKCheckBox* ptr, bool enable);

    void updateBlendType();
     
private:
    std::array<BKCheckBox*, 4> mChannelMaskCheckBoies;

    Ogre::HlmsBlendblock mBlendblock;

    Ogre::SceneBlendType mBlendType = Ogre::SceneBlendType::SBT_REPLACE;

    bool mbAlphaBlendTypeEnable = true;

    Ogre::SceneBlendType mAlphaBlendType = Ogre::SceneBlendType::SBT_REPLACE;
};

Q_DECLARE_METATYPE(Ogre::HlmsBlendblock)