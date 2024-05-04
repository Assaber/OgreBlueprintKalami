#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsSamplerblock.h"
#include <limits>
#include <array>
#include <QDebug>

class BKCell;
class PbsDetailCard : public BKCard
{
    CREATE_CARD_FACTORY(PbsDetailCard)

public:
    struct DetailInfo
    {
        Ogre::PbsTextureTypes type = Ogre::PBSM_DETAIL0;
        Ogre::HlmsSamplerblock sampler;
        Ogre::String texture = "";
        int8_t uv = -1;
        uint8_t indexOffset = -1;
        Ogre::PbsBlendModes blendMode = Ogre::PbsBlendModes::NUM_PBSM_BLEND_MODES;
        bool offsetScaleEnable = false;
        std::array<float, 4> offsetScale = {0, 0, 1.0f, 1.0f};
        
        operator QVariant() const {
            return QVariant::fromValue(*this);
        }

        bool operator <(const DetailInfo& o) const {
            return type < o.type;
        }

        bool operator ==(const DetailInfo& o) const {
            return type == o.type;
        }
    };

public:
    PbsDetailCard();
    virtual QVariant getCurrentCardValue() override;

private:
    int8_t getOffsetByTextureType(Ogre::PbsTextureTypes type);

private:
    DetailInfo mDetailInfo;
    BKCell* mpOutputCell = nullptr;
};

Q_DECLARE_METATYPE(PbsDetailCard::DetailInfo)