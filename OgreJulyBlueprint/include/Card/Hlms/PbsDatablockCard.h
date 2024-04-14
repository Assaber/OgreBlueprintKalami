#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbsDatablock.h"
#include "Hlms/PbsMapCard.h"

class BKLineEdit;
class BKCell;
class PbsDatablockCard : public BKCard
{
    CREATE_CARD_FACTORY(PbsDatablockCard)

public:
    PbsDatablockCard();
    ~PbsDatablockCard();

public:
    virtual QVariant getCurrentCardValue() override;

private:
    void createHlms(bool recreate = false);

private:
    Ogre::String mstrName = "";
    Ogre::String mstrOldName = "";
    Ogre::HlmsPbs* mpPbs = nullptr;
    Ogre::HlmsPbsDatablock* mpDatablock = nullptr;
    Ogre::HlmsMacroblock mMacroblock;
    Ogre::HlmsBlendblock mBlendblock;
    std::vector<PbsMapCard::TexInfo> mTextureInfoVec;

    QColor mBackgroundColor;
    QColor mDiffuse;
    QColor mSpecular;
    QColor mEmissive;
    QColor mFresnel;
    float mRoughness = 1.0f;

    BKLineEdit* mpNameLineEdit = nullptr;
    BKCell* mpOutputCell = nullptr;
    BKAnchor* mpTextureMapInputAnchor = nullptr;
};

// Q_DECLARE_METATYPE(Ogre::HlmsPbsDatablock)