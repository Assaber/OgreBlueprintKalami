#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbs.h"

class BKCell;
class PbsMapCard : public BKCard
{
    CREATE_CARD_FACTORY(PbsMapCard)

public:
    struct TexInfo
    {
        Ogre::PbsTextureTypes type;
        Ogre::String texture;
        uint8_t uv = -1;

        operator QVariant() const
        {
            return QVariant::fromValue(*this);
        }
    };

public:
    PbsMapCard();

    virtual QVariant getCurrentCardValue() override;
     
private:
    TexInfo mTextureInfo;
    BKCell* mpOutputCell = nullptr;
};

Q_DECLARE_METATYPE(PbsMapCard::TexInfo)