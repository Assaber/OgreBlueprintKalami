#pragma once
#include "container/BKCard.h"
#include "OgreHlmsSamplerblock.h"
#include "ResetResourceSup.h"
#include "OgreHlmsPbs.h"
#include <QDebug>

class BKCell;
class PbsCommonTextureCard : public ResetResourceSup
{
    CREATE_CARD_FACTORY(PbsCommonTextureCard)

public:
    struct Info
    {
        Ogre::PbsTextureTypes type = Ogre::PBSM_DIFFUSE;
        Ogre::String texture =  "";
        int8_t uv = -1;
        Ogre::HlmsSamplerblock sampler;

        operator QVariant() const {
            return QVariant::fromValue(*this);
        }

        bool operator <(const Info& o) const {
            return type < o.type;
        }

        bool operator ==(const Info& o) const {
            return type == o.type;
        }
    };

public:
    PbsCommonTextureCard();
    virtual QVariant getCurrentCardValue() override;

private:
    Info mTextureInfo;
};

Q_DECLARE_METATYPE(PbsCommonTextureCard::Info)