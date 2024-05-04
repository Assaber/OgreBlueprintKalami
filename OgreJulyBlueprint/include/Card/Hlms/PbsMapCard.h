#pragma once
#include "container/BKCard.h"
#include "OgreHlmsSamplerblock.h"
#include "OgreHlmsPbs.h"
#include <QDebug>

class BKCell;
class PbsMapCard : public BKCard
{
    CREATE_CARD_FACTORY(PbsMapCard)

public:
    struct TexInfo
    {
        Ogre::PbsTextureTypes type = Ogre::PBSM_DIFFUSE;
        Ogre::String texture =  "";
        int8_t uv = -1;
        Ogre::HlmsSamplerblock sampler;

        operator QVariant() const {
            return QVariant::fromValue(*this);
        }

        bool operator <(const TexInfo& o) const {
            return type < o.type;
        }

        bool operator ==(const TexInfo& o) const {
            return type == o.type;
        }
    };

public:
    PbsMapCard();
    virtual QVariant getCurrentCardValue() override;

private:
    void resetResourceDir(const Ogre::String& filepath);
     
private:
    TexInfo mTextureInfo;
    BKCell* mpOutputCell = nullptr;
};

Q_DECLARE_METATYPE(PbsMapCard::TexInfo)