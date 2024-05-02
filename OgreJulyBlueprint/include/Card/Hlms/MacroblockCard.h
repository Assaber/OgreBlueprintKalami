#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbsDatablock.h"

class BKCell;
class MacroblockCard : public BKCard
{
    CREATE_CARD_FACTORY(MacroblockCard)

public:
    MacroblockCard();
    virtual QVariant getCurrentCardValue() override;
 
private:
    Ogre::HlmsMacroblock mMacroblock;
    
};

Q_DECLARE_METATYPE(Ogre::HlmsMacroblock)