#pragma once
#include "container/BKCard.h"
#include "OgreHlmsPbsDatablock.h"
#include "Hlms/PbsCommonTextureCard.h"
#include "Hlms/PbsDetailTextureCard.h"
#include "Hlms/PbsTransparentCard.h"

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
    Ogre::String mstrName = "snow";
    Ogre::String mstrOldName = "";
    Ogre::HlmsPbs* mpPbs = nullptr;
    Ogre::HlmsPbsDatablock* mpDatablock = nullptr;
    Ogre::HlmsMacroblock mMacroblock;
    Ogre::HlmsBlendblock mBlendblock;
    std::set<PbsCommonTextureCard::Info> mTextureInfoSet;
    std::set<PbsDetailTextureCard::Info> mDetailInfoSet;
    PbsTransparentCard::Info mTransparentInfo;

    QColor mBackgroundColor;
    QColor mDiffuse;
    QColor mSpecular;
    QColor mEmissive;
    QColor mFresnel;
    float mRoughness = 1.0f;

    BKLineEdit* mpNameLineEdit = nullptr;
    BKCell* mpOutputCell = nullptr;
    BKAnchor* mpTextureMapInputAnchor = nullptr;
    BKAnchor* mpTextureDetailAnchor = nullptr;
};