#include "Application.h"
#include "OgreBlueprint.h"

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
        menu->registCard<TestCard>("功能", "测试");
        menu->registCard<PrintCard>("功能", "理论上万物皆可打印");
        menu->registCard<ReadFileCard>("功能", "文件读取");

        menu->registCard<RenderItemCard>("靶子", "渲染对象");

        menu->registCard<PbsDatablockCard>("材质", "PBS数据块");
        menu->registCard<PbsCommonTextureCard>("材质", "PBS材质");
        menu->registCard<PbsDetailTextureCard>("材质", "PBS细节");
        menu->registCard<PbsTransparentCard>("材质", "PBS透明");
        menu->registCard<BlendblockCard>("材质", "混合块");
        menu->registCard<MacroblockCard>("材质", "宏块");
        menu->registCard<UnlitDatablockCard>("材质", "Unlit数据块");
        menu->registCard<UnlitDiffuseMapCard>("材质", "漫反射贴图");

        menu->registCard<SimpleMaterialCard>("材质", "普通材质");
        menu->registCard<SimpleFragmentProgCard>("材质", "普通片段程序");
        menu->registCard<SimpleVertexProgCard>("材质", "普通顶点程序");

#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
        menu->registCard<ParticleCard>("粒子", "金色传说");
        menu->registCard<ParticleBillboardSettingCard>("粒子", "广告板设置");
        menu->registCard<ParticleEmitterCard>("粒子", "发射器");
        menu->registCard<ParticleEmitterTypeCard>("粒子", "发射器类型扩展");
        menu->registCard<ParticleColourFader2Card>("粒子", "双重颜色渐变导演");
        menu->registCard<ParticleColourFaderCard>("粒子", "颜色渐变导演");
        menu->registCard<ParticleColourImageCard>("粒子", "图片渐变导演");
        menu->registCard<ParticleColourInterpolatorCard>("粒子", "多重颜色渐变导演");
        menu->registCard<ParticleDeflectorPlaneCard>("粒子", "金色传说导演");
        menu->registCard<ParticleDirectionRandomiserCard>("粒子", "方向随机导演");
        menu->registCard<ParticleLinearForceCard>("粒子", "线性力导演");
        menu->registCard<ParticleRotatorCard>("粒子", "旋转导演");
        menu->registCard<ParticleScalerCard>("粒子", "缩放导演");
#endif
    }
    
    b.showMaximized();

    int ret = a.join(std::bind(&OgreBlueprint::drive, &b));

    return ret;
}