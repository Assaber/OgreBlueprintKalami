﻿#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbsDatablock.h"

class BKCell;
class PbsTransparentCard : public BKCard
{
    CREATE_CARD_FACTORY(PbsTransparentCard)

public:
    struct Info
    {
        bool enable = false;
        float transparency = 1.0f;
        Ogre::HlmsPbsDatablock::TransparencyModes mode = Ogre::HlmsPbsDatablock::Transparent;
        bool alphaFromTex = true;

        operator QVariant() const
        {
            return QVariant::fromValue(*this);
        }
    };

public:
    PbsTransparentCard();
    ~PbsTransparentCard();

    virtual QVariant getCurrentCardValue() override;
     
private:
    Info mInfo;
    BKCell* mpOutputCell = nullptr;
};

Q_DECLARE_METATYPE(PbsTransparentCard::Info)