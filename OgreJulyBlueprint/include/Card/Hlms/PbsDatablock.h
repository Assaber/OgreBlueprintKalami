#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbsDatablock.h"

class BKLineEdit;
class BKCell;
class PbsDatablockCard : public BKCard
{
    CREATE_CARD_FACTORY(PbsDatablockCard)

public:
    PbsDatablockCard();
    ~PbsDatablockCard();

private:
    void createHlms(bool recreate = false);

private:
    Ogre::String mstrName = "";
    Ogre::String mstrOldName = "";
    Ogre::HlmsPbs* mpPbs = nullptr;
    Ogre::HlmsPbsDatablock* mpDatablock = nullptr;
    Ogre::HlmsMacroblock mMacroblock;
    Ogre::HlmsBlendblock mBlendblock;

    QColor mBackgroundColor;

    BKLineEdit* mpNameLineEdit = nullptr;
    BKCell* mpOutputCell = nullptr;
};

// Q_DECLARE_METATYPE(Ogre::HlmsPbsDatablock)