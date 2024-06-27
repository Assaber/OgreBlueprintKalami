#pragma once
#include "container/BKCard.h"
#include "ResetResourceSup.h"
#include "OgreHlmsSamplerblock.h"
#include "OgreHlmsUnlit.h"
#include <QDebug>

class BKCell;
class UnlitDiffuseMapCard : public ResetResourceSup
{
    CREATE_CARD_FACTORY(UnlitDiffuseMapCard)

public:
    struct Info
    {
        uint8_t index = 0;          // 0 ~ Ogre::UnlitTextureTypes::NUM_UNLIT_TEXTURE_TYPES - 1
        Ogre::UnlitBlendModes blendMode = Ogre::UNLIT_BLEND_NORMAL_PREMUL;
        Ogre::String texture =  "";
        int8_t uv = -1;

        operator QVariant() const {
            return QVariant::fromValue(*this);
        }

        bool operator <(const Info& o) const {
            return index < o.index;
        }

        bool operator ==(const Info& o) const {
            return index == o.index;
        }
    };

public:
    UnlitDiffuseMapCard();
    virtual QVariant getCurrentCardValue() override;
     
private:
    Info mTextureInfo;
    QString mLastTextureName = "";
};

Q_DECLARE_METATYPE(UnlitDiffuseMapCard::Info)