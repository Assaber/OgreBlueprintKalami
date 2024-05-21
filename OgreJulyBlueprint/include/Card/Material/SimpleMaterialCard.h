#pragma once
#include "container/BKCard.h"
#include "OgreString.h"
#include "SimpleMaterialProgramCard.h"

class BKAnchor;
class SimpleMaterialCard : public BKCard
{
    CREATE_CARD_FACTORY(SimpleMaterialCard)

public:
    SimpleMaterialCard();
    virtual QVariant getCurrentCardValue() override;

private:
    void createMaterial();
    void updateMaterial();

    bool slotVertexProgramUpdate(const QVariant& vd);
    bool slotFragmentProgramUpdate(const QVariant& fd);

    static void transConstantInfo2Program(const SimpleMaterialProgramCard::ConstantInfo& src, Ogre::GpuProgramParametersSharedPtr dst);

private:
    BKAnchor* mpOutputAnchor = nullptr;
    Ogre::String mstrMaterialName;
    SimpleVertexProgCard::ProgramInfo mVertexProgramInfo;
    SimpleFragmentProgCard::ProgramInfo mFragmentProgramInfo;
};