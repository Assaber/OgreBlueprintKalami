#include "Application.h"
#include "OgreBlueprint.h"

#include "BKCreator.h"
#include "TestCard.h"
#include "PrintCard.h"
#include "RenderItemCard.h"
#include "Card/Hlms/PbsDatablockCard.h"
#include "Card/Hlms/PbsDetailTextureCard.h"
#include "Card/Hlms/PbsTransparentCard.h"
#include "Card/Hlms/PbsCommonTextureCard.h"
#include "Card/Hlms/BlendblockCard.h"
#include "Card/Hlms/MacroblockCard.h"
#include "Card/Hlms/UnlitDatablockCard.h"
#include "Card/Hlms/UnlitDiffuseMapCard.h"
#include "Card/Material/SimpleMaterialCard.h"
#include "Card/Material/SimpleMaterialProgramCard.h"
#include "Card/ReadFileCard.h"

#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Card/Particle/ParticleCard.h"
#include "Card/Particle/ParticleBillboardSettingCard.h"
#include "Card/Particle/ParticleEmitterCard.h"
#include "Card/Particle/ParticleEmitterTypeCard.h"
#include "Card/Particle/Affector/ParticleColourFader2Card.h"
#include "Card/Particle/Affector/ParticleColourFaderCard.h"
#include "Card/Particle/Affector/ParticleColourImageCard.h"
#include "Card/Particle/Affector/ParticleColourInterpolatorCard.h"
#include "Card/Particle/Affector/ParticleDeflectorPlaneCard.h"
#include "Card/Particle/Affector/ParticleDirectionRandomiserCard.h"
#include "Card/Particle/Affector/ParticleLinearForceCard.h"
#include "Card/Particle/Affector/ParticleRotatorCard.h"
#include "Card/Particle/Affector/ParticleScalerCard.h"
#endif

#include <functional>
#include <QDateTime>

int main(int argc, char** argv)
{
    Application a(argc, argv);

    qsrand(QDateTime::currentSecsSinceEpoch());

    // 注册可以被识别的卡片
    BKCreator::registCard<TestCard>();
    BKCreator::registCard<PrintCard>();
    BKCreator::registCard<RenderItemCard>();
    BKCreator::registCard<PbsDatablockCard>();
    BKCreator::registCard<PbsCommonTextureCard>();
    BKCreator::registCard<PbsDetailTextureCard>();
    BKCreator::registCard<PbsTransparentCard>();
    BKCreator::registCard<BlendblockCard>();
    BKCreator::registCard<MacroblockCard>();
    BKCreator::registCard<UnlitDatablockCard>();
    BKCreator::registCard<UnlitDiffuseMapCard>();
    BKCreator::registCard<SimpleMaterialCard>();
    BKCreator::registCard<ReadFileCard>();
    BKCreator::registCard<SimpleFragmentProgCard>();
    BKCreator::registCard<SimpleVertexProgCard>();

    // 注册自定义结构体的锚点识别及颜色
    BKAnchor::registDataType(QMetaTypeId<PbsCommonTextureCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<PbsDetailTextureCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<PbsTransparentCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<Ogre::HlmsBlendblock>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<Ogre::HlmsMacroblock>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<UnlitDiffuseMapCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<SimpleFragmentProgCard::ProgramInfo>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<SimpleVertexProgCard::ProgramInfo>::qt_metatype_id());

#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
    BKCreator::registCard<ParticleCard>();
    BKCreator::registCard<ParticleBillboardSettingCard>();
    BKCreator::registCard<ParticleEmitterCard>();
    BKCreator::registCard<ParticleEmitterTypeCard>();
    BKCreator::registCard<ParticleColourFader2Card>();
    BKCreator::registCard<ParticleColourFaderCard>();
    BKCreator::registCard<ParticleColourImageCard>();
    BKCreator::registCard<ParticleColourInterpolatorCard>();
    BKCreator::registCard<ParticleDeflectorPlaneCard>();
    BKCreator::registCard<ParticleDirectionRandomiserCard>();
    BKCreator::registCard<ParticleLinearForceCard>();
    BKCreator::registCard<ParticleRotatorCard>();
    BKCreator::registCard<ParticleScalerCard>();

    BKAnchor::registDataType(QMetaTypeId<ParticleBillboardSettingCard::Data>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<ParticleEmitterCard::Data>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<ParticleEmitterTypeCard::Data>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<particle::ParticleAffector>::qt_metatype_id());
#endif
  
    OgreBlueprint b;
    b.showMaximized();

    int ret = a.join(std::bind(&OgreBlueprint::drive, &b));

    return ret;
}