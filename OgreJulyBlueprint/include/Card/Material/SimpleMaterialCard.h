#pragma once
#include "container/BKCard.h"
#include "OgreString.h"

class BKAnchor;
class SimpleMaterialCard : public BKCard
{
    CREATE_CARD_FACTORY(SimpleMaterialCard)

public:
    SimpleMaterialCard();
    virtual QVariant getCurrentCardValue() override;

private:
    void createMaterial();
    // void createMaterial2();

private:
    void recreateMaterial();
    bool slotProgramTypeChanged(const QVariant& type);
    void slotAutoParamCountChanged(size_t count, const QVariantList& params);
    void slotAutoParamDataChanged(size_t count, const QVariantList& params);

private:
    BKAnchor* mpOutputAnchor = nullptr;
    Ogre::String mstrMaterialName;
};