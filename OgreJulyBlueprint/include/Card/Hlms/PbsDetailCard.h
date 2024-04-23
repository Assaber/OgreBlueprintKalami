#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsSamplerblock.h"
#include <limits>
#include <array>

class BKCell;
class PbsDetailCard : public BKCard
{
    CREATE_CARD_FACTORY(PbsDetailCard)

public:
    struct DetailInfo
    {
        Ogre::PbsTextureTypes textureType;
        Ogre::HlmsSamplerblock sampler;
        Ogre::String texture = "";
        uint8_t uv = -1;
        uint8_t indexOffset;
        Ogre::PbsBlendModes blendMode = Ogre::PbsBlendModes::NUM_PBSM_BLEND_MODES;
        bool offsetScaleEnable = false;
        std::array<float, 4> offsetScale = {0, 0, 1.0f, 1.0f};
        

        operator QVariant() const
        {
            return QVariant::fromValue(*this);
        }
    };

public:
    PbsDetailCard();

    virtual QVariant getCurrentCardValue() override;

private:
    Ogre::TextureAddressingMode getTAMByName(const char* name) const;

     
private:
    DetailInfo mDetailInfo;
    BKCell* mpOutputCell = nullptr;
};

Q_DECLARE_METATYPE(PbsDetailCard::DetailInfo)