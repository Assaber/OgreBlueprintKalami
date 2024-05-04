#pragma once
#include "container/BKCard.h"
#include "OgreHlmsUnlitDatablock.h"
#include "Hlms/UnlitDiffuseMapCard.h"

class BKLineEdit;
class BKCell;
namespace Ogre
{
    class SceneNode;
    class SubItem;
};

class UnlitDatablockCard : public BKCard
{
    CREATE_CARD_FACTORY(UnlitDatablockCard)

public:
    UnlitDatablockCard();
    ~UnlitDatablockCard();

public:
    virtual QVariant getCurrentCardValue() override;

private:
    void createHlms(bool recreate = false);
    void getHitSubItems(const Ogre::IdString& id, Ogre::SceneNode* node, std::set<Ogre::SubItem*>& items);

private:
    Ogre::String mstrName = "usnow";
    Ogre::String mstrOldName = "";
    Ogre::HlmsUnlit* mpUnlit = nullptr;
    Ogre::HlmsUnlitDatablock* mpDatablock = nullptr;
    Ogre::HlmsMacroblock mMacroblock;
    Ogre::HlmsBlendblock mBlendblock;
    std::set<UnlitDiffuseMapCard::Info> mTextureInfoSet;

    QColor mDiffuse = Qt::white;
    Ogre::String mstrDiffuseTexture = "";
    int8_t mDiffuseUV = 0;

    BKLineEdit* mpNameLineEdit = nullptr;
    BKCell* mpOutputCell = nullptr;
};