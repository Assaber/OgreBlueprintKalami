#include "Application.h"
#include "OgreBlueprint.h"
#include "GlobalSettingsWidget.h"
#include "AppConfig.h"

#include "BKCreator.hpp"
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
#include "Card/CanvasCard.h"

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

#include "CardFilterComboBox.h"

int main(int argc, char** argv)
{
    Application a(argc, argv);

    qsrand(QDateTime::currentSecsSinceEpoch());

    if (!AppConfig::getSingleton()->loadConfig())
    {
        GlobalSettingsWidget w;
        if (w.exec() == QDialog::Rejected)
            return 0;
    }

    // Register
    BKAnchor::registDataType(QMetaTypeId<PbsCommonTextureCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<PbsDetailTextureCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<PbsTransparentCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<Ogre::HlmsBlendblock>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<Ogre::HlmsMacroblock>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<UnlitDiffuseMapCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<SimpleFragmentProgCard::ProgramInfo>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<SimpleVertexProgCard::ProgramInfo>::qt_metatype_id());

#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
    BKAnchor::registDataType(QMetaTypeId<ParticleBillboardSettingCard::Data>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<ParticleEmitterCard::Data>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<ParticleEmitterTypeCard::Data>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<particle::ParticleAffector>::qt_metatype_id());
#endif

    OgreBlueprint b;
    BlueprintEditor* editor = b.getBlueprintEditorPtr();
    if (editor)
    {
        CardFilterComboBox* menu = editor->getFilterMenuPtr();
        menu->registCard<TestCard>("Common", "Test");
        menu->registCard<PrintCard>("Common", "Print");
        menu->registCard<ReadFileCard>("Common", "Read File");

        menu->registCard<RenderItemCard>("Render", "Render Item");

        menu->registCard<PbsDatablockCard>("Texture", "PBS Datablock");
        menu->registCard<PbsCommonTextureCard>("Texture", "PBS Texture");
        menu->registCard<PbsDetailTextureCard>("Texture", "PBS Detail");
        menu->registCard<PbsTransparentCard>("Texture", "PBS Transparent");
        menu->registCard<BlendblockCard>("Texture", "Blendblock");
        menu->registCard<MacroblockCard>("Texture", "Macroblock");
        menu->registCard<UnlitDatablockCard>("Texture", "Unlit Datablock");
        menu->registCard<UnlitDiffuseMapCard>("Texture", "Unlit Diffuse");

        menu->registCard<SimpleMaterialCard>("Texture", "Simple Material");
        menu->registCard<SimpleFragmentProgCard>("Texture", "Simple Fragment Program");
        menu->registCard<SimpleVertexProgCard>("Texture", "Simple Vertex Program");

        menu->registCard<CanvasCard>("Texture", "Canvas");

#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
        menu->registCard<ParticleCard>("Particle", "Particle");
        menu->registCard<ParticleBillboardSettingCard>("Particle", "Billboard Setting");
        menu->registCard<ParticleEmitterCard>("Particle", "Emitter");
        menu->registCard<ParticleEmitterTypeCard>("Particle", "Emitter Type");
        menu->registCard<ParticleColourFader2Card>("Particle", "Colour Fader Affector");
        menu->registCard<ParticleColourFaderCard>("Particle", "Colour Fader Affector");
        menu->registCard<ParticleColourImageCard>("Particle", "Colour Image Affector");
        menu->registCard<ParticleColourInterpolatorCard>("Particle", "Colour Interpolator Affector");
        menu->registCard<ParticleDeflectorPlaneCard>("Particle", "Deflector Plane Affector");
        menu->registCard<ParticleDirectionRandomiserCard>("Particle", "Direction Randomiser Affector");
        menu->registCard<ParticleLinearForceCard>("Particle", "Linear Force Affector");
        menu->registCard<ParticleRotatorCard>("Particle", "Rotator Affector");
        menu->registCard<ParticleScalerCard>("Particle", "Scaler Affector");
#endif
    }
    
    b.showMaximized();

    int ret = a.join(std::bind(&OgreBlueprint::drive, &b));

    return ret;
}