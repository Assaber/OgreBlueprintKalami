#include "Material/SimpleMaterialCard.h"

#include "BKCreator.h"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKPushButton.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSpacer.h"

#include "OgreHighLevelGpuProgramManager.h"
#include "OgreHighLevelGpuProgram.h"
#include "OgreMaterialManager.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgrePass.h"

#include "OgreScriptCompiler.h"

// 正则天下第一！
static std::map<Ogre::String, Ogre::GpuProgramParameters::AutoConstantType> name2AutoConstType = {
    { "World Matrix", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX },
    { "Inverse World Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_WORLD_MATRIX },
    { "Transpose World Matrix", Ogre::GpuProgramParameters::ACT_TRANSPOSE_WORLD_MATRIX },
    { "Inverse Transpose World Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_TRANSPOSE_WORLD_MATRIX },
    { "World Matrix Array 3x4", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX_ARRAY_3x4 },
    { "World Matrix Array", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX_ARRAY },
    { "World Dualquaternion Array 2x4", Ogre::GpuProgramParameters::ACT_WORLD_DUALQUATERNION_ARRAY_2x4 },
    { "World Scale Shear Matrix Array 3x4", Ogre::GpuProgramParameters::ACT_WORLD_SCALE_SHEAR_MATRIX_ARRAY_3x4 },
    { "View Matrix", Ogre::GpuProgramParameters::ACT_VIEW_MATRIX },
    { "Inverse View Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_VIEW_MATRIX },
    { "Transpose View Matrix", Ogre::GpuProgramParameters::ACT_TRANSPOSE_VIEW_MATRIX },
    { "Inverse Transpose View Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_TRANSPOSE_VIEW_MATRIX },
    { "Projection Matrix", Ogre::GpuProgramParameters::ACT_PROJECTION_MATRIX },
    { "Inverse Projection Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_PROJECTION_MATRIX },
    { "Transpose Projection Matrix", Ogre::GpuProgramParameters::ACT_TRANSPOSE_PROJECTION_MATRIX },
    { "Inverse Transpose Projection Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_TRANSPOSE_PROJECTION_MATRIX },
    { "Viewproj Matrix", Ogre::GpuProgramParameters::ACT_VIEWPROJ_MATRIX },
    { "Inverse Viewproj Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_VIEWPROJ_MATRIX },
    { "Transpose Viewproj Matrix", Ogre::GpuProgramParameters::ACT_TRANSPOSE_VIEWPROJ_MATRIX },
    { "Inverse Transpose Viewproj Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_TRANSPOSE_VIEWPROJ_MATRIX },
    { "Worldview Matrix", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX },
    { "Inverse Worldview Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEW_MATRIX },
    { "Transpose Worldview Matrix", Ogre::GpuProgramParameters::ACT_TRANSPOSE_WORLDVIEW_MATRIX },
    { "Inverse Transpose Worldview Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX },
    { "Worldviewproj Matrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX },
    { "Inverse Worldviewproj Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEWPROJ_MATRIX },
    { "Transpose Worldviewproj Matrix", Ogre::GpuProgramParameters::ACT_TRANSPOSE_WORLDVIEWPROJ_MATRIX },
    { "Inverse Transpose Worldviewproj Matrix", Ogre::GpuProgramParameters::ACT_INVERSE_TRANSPOSE_WORLDVIEWPROJ_MATRIX },
    { "Render Target Flipping", Ogre::GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING },
    { "Vertex Winding", Ogre::GpuProgramParameters::ACT_VERTEX_WINDING },
    { "Fog Colour", Ogre::GpuProgramParameters::ACT_FOG_COLOUR },
    { "Fog Params", Ogre::GpuProgramParameters::ACT_FOG_PARAMS },
    { "Surface Ambient Colour", Ogre::GpuProgramParameters::ACT_SURFACE_AMBIENT_COLOUR },
    { "Surface Diffuse Colour", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR },
    { "Surface Specular Colour", Ogre::GpuProgramParameters::ACT_SURFACE_SPECULAR_COLOUR },
    { "Surface Emissive Colour", Ogre::GpuProgramParameters::ACT_SURFACE_EMISSIVE_COLOUR },
    { "Surface Shininess", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS },
    { "Surface Alpha Rejection Value", Ogre::GpuProgramParameters::ACT_SURFACE_ALPHA_REJECTION_VALUE },
    { "Light Count", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT },
    { "Ambient Light Colour", Ogre::GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR },
    { "Light Diffuse Colour", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR },
    { "Light Specular Colour", Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR },
    { "Light Attenuation", Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION },
    { "Spotlight Params", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_PARAMS },
    { "Light Position", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION },
    { "Light Position Object Space", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE },
    { "Light Position View Space", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_VIEW_SPACE },
    { "Light Direction", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION },
    { "Light Direction Object Space", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_OBJECT_SPACE },
    { "Light Direction View Space", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_VIEW_SPACE },
    { "Light Distance Object Space", Ogre::GpuProgramParameters::ACT_LIGHT_DISTANCE_OBJECT_SPACE },
    { "Light Power Scale", Ogre::GpuProgramParameters::ACT_LIGHT_POWER_SCALE },
    { "Light Diffuse Colour Power Scaled", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED },
    { "Light Specular Colour Power Scaled", Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED },
    { "Light Diffuse Colour Array", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_ARRAY },
    { "Light Specular Colour Array", Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR_ARRAY },
    { "Light Diffuse Colour Power Scaled Array",Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY },
    { "Light Specular Colour Power Scaled Array", Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY, },
    { "Light Attenuation Array", Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION_ARRAY },
    { "Light Position Array", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_ARRAY },
    { "Light Position Object Space Array", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY },
    { "Light Position View Space Array", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_VIEW_SPACE_ARRAY },
    { "Light Direction Array", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_ARRAY },
    { "Light Direction Object Space Array", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_OBJECT_SPACE_ARRAY },
    { "Light Direction View Space Array", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_VIEW_SPACE_ARRAY },
    { "Light Distance Object Space Array", Ogre::GpuProgramParameters::ACT_LIGHT_DISTANCE_OBJECT_SPACE_ARRAY },
    { "Light Power Scale Array", Ogre::GpuProgramParameters::ACT_LIGHT_POWER_SCALE_ARRAY },
    { "Spotlight Params Array", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_PARAMS_ARRAY },
    { "Derived Ambient Light Colour", Ogre::GpuProgramParameters::ACT_DERIVED_AMBIENT_LIGHT_COLOUR },
    { "Derived Scene Colour", Ogre::GpuProgramParameters::ACT_DERIVED_SCENE_COLOUR },
    { "Derived Light Diffuse Colour", Ogre::GpuProgramParameters::ACT_DERIVED_LIGHT_DIFFUSE_COLOUR },
    { "Derived Light Specular Colour", Ogre::GpuProgramParameters::ACT_DERIVED_LIGHT_SPECULAR_COLOUR },
    { "Derived Light Diffuse Colour Array", Ogre::GpuProgramParameters::ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY },
    { "Derived Light Specular Colour Array", Ogre::GpuProgramParameters::ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY },
    { "Light Casts Shadows", Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS },
    { "Light Casts Shadows Array", Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY },
    { "Shadow Extrusion Distance", Ogre::GpuProgramParameters::ACT_SHADOW_EXTRUSION_DISTANCE },
    { "Camera Position", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION },
    { "Camera Position Object Space", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE },
    { "Texture Viewproj Matrix", Ogre::GpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX },
    { "Texture Viewproj Matrix Array", Ogre::GpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX_ARRAY },
    { "Texture Worldviewproj Matrix", Ogre::GpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX },
    { "Texture Worldviewproj Matrix Array", Ogre::GpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY },
    { "Spotlight Viewproj Matrix", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_VIEWPROJ_MATRIX },
    { "Spotlight Viewproj Matrix Array", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_VIEWPROJ_MATRIX_ARRAY },
    { "Spotlight Worldviewproj Matrix", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX },
    { "Spotlight Worldviewproj Matrix Array", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX_ARRAY },
    { "Rs Depth Range", Ogre::GpuProgramParameters::ACT_RS_DEPTH_RANGE },
    { "Custom", Ogre::GpuProgramParameters::ACT_CUSTOM },
    { "Time", Ogre::GpuProgramParameters::ACT_TIME },
    { "Time 0 X", Ogre::GpuProgramParameters::ACT_TIME_0_X },
    { "Costime 0 X", Ogre::GpuProgramParameters::ACT_COSTIME_0_X },
    { "Sintime 0 X", Ogre::GpuProgramParameters::ACT_SINTIME_0_X },
    { "Tantime 0 X", Ogre::GpuProgramParameters::ACT_TANTIME_0_X },
    { "Time 0 X Packed", Ogre::GpuProgramParameters::ACT_TIME_0_X_PACKED },
    { "Time 0 1", Ogre::GpuProgramParameters::ACT_TIME_0_1 },
    { "Costime 0 1", Ogre::GpuProgramParameters::ACT_COSTIME_0_1 },
    { "Sintime 0 1", Ogre::GpuProgramParameters::ACT_SINTIME_0_1 },
    { "Tantime 0 1", Ogre::GpuProgramParameters::ACT_TANTIME_0_1 },
    { "Time 0 1 Packed", Ogre::GpuProgramParameters::ACT_TIME_0_1_PACKED },
    { "Time 0 2pi", Ogre::GpuProgramParameters::ACT_TIME_0_2PI },
    { "Costime 0 2pi", Ogre::GpuProgramParameters::ACT_COSTIME_0_2PI },
    { "Sintime 0 2pi", Ogre::GpuProgramParameters::ACT_SINTIME_0_2PI },
    { "Tantime 0 2pi", Ogre::GpuProgramParameters::ACT_TANTIME_0_2PI },
    { "Time 0 2pi Packed", Ogre::GpuProgramParameters::ACT_TIME_0_2PI_PACKED },
    { "Frame Time", Ogre::GpuProgramParameters::ACT_FRAME_TIME },
    { "Fps", Ogre::GpuProgramParameters::ACT_FPS },
    { "Viewport Width", Ogre::GpuProgramParameters::ACT_VIEWPORT_WIDTH },
    { "Viewport Height", Ogre::GpuProgramParameters::ACT_VIEWPORT_HEIGHT },
    { "Inverse Viewport Width", Ogre::GpuProgramParameters::ACT_INVERSE_VIEWPORT_WIDTH },
    { "Inverse Viewport Height", Ogre::GpuProgramParameters::ACT_INVERSE_VIEWPORT_HEIGHT },
    { "Viewport Size", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE },
    { "View Direction", Ogre::GpuProgramParameters::ACT_VIEW_DIRECTION },
    { "View Side Vector", Ogre::GpuProgramParameters::ACT_VIEW_SIDE_VECTOR },
    { "View Up Vector", Ogre::GpuProgramParameters::ACT_VIEW_UP_VECTOR },
    { "Fov", Ogre::GpuProgramParameters::ACT_FOV },
    { "Near Clip Distance", Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE },
    { "Far Clip Distance", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE },
    { "Pass Number", Ogre::GpuProgramParameters::ACT_PASS_NUMBER },
    { "Pass Iteration Number", Ogre::GpuProgramParameters::ACT_PASS_ITERATION_NUMBER },
    { "Animation Parametric", Ogre::GpuProgramParameters::ACT_ANIMATION_PARAMETRIC },
    { "Texel Offsets", Ogre::GpuProgramParameters::ACT_TEXEL_OFFSETS },
    { "Scene Depth Range", Ogre::GpuProgramParameters::ACT_SCENE_DEPTH_RANGE },
    { "Shadow Scene Depth Range", Ogre::GpuProgramParameters::ACT_SHADOW_SCENE_DEPTH_RANGE },
    { "Shadow Scene Depth Range Array", Ogre::GpuProgramParameters::ACT_SHADOW_SCENE_DEPTH_RANGE_ARRAY },
    { "Shadow Colour", Ogre::GpuProgramParameters::ACT_SHADOW_COLOUR },
    { "Uav Size", Ogre::GpuProgramParameters::ACT_UAV_SIZE },
    { "Inverse Uav Size", Ogre::GpuProgramParameters::ACT_INVERSE_UAV_SIZE },
    { "Packed Uav Size", Ogre::GpuProgramParameters::ACT_PACKED_UAV_SIZE },
    { "Texture Size", Ogre::GpuProgramParameters::ACT_TEXTURE_SIZE },
    { "Inverse Texture Size", Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE },
    { "Packed Texture Size", Ogre::GpuProgramParameters::ACT_PACKED_TEXTURE_SIZE },
    { "Texture Matrix", Ogre::GpuProgramParameters::ACT_TEXTURE_MATRIX },
    { "Lod Camera Position", Ogre::GpuProgramParameters::ACT_LOD_CAMERA_POSITION },
    { "Lod Camera Position Object Space", Ogre::GpuProgramParameters::ACT_LOD_CAMERA_POSITION_OBJECT_SPACE },
    { "Light Custom", Ogre::GpuProgramParameters::ACT_LIGHT_CUSTOM },
    { "Pssm Splits", Ogre::GpuProgramParameters::ACT_PSSM_SPLITS },
    { "Pssm Blends", Ogre::GpuProgramParameters::ACT_PSSM_BLENDS },
    { "Pssm Fade", Ogre::GpuProgramParameters::ACT_PSSM_FADE },
};

enum ParamType : uint8_t
{
    Real,
    Int,
    UInt,
    Boolean,
    Vector4,
    Vector3,
    Vector2,
    Matrix3,
    Matrix4,
};

static std::map<Ogre::String, ParamType> name2ParamType = {
    {"Real", ParamType::Real},
    {"Int", ParamType::Int},
    {"UInt", ParamType::UInt},
    {"Bool", ParamType::Boolean},
    {"Vector4", ParamType::Vector4},
    {"Vector3", ParamType::Vector3},
    {"Vector2", ParamType::Vector2},
    {"Matrix3", ParamType::Matrix3},
    {"Matrix4", ParamType::Matrix4},
};

SimpleMaterialCard::SimpleMaterialCard()
{
    setTitle("简单材质");

    BKLabel* outputLabel = BKCreator::create<BKLabel>();
    outputLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    outputLabel->setText("输出");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, BKAnchor::String);
    outputCell->append(outputLabel, false);;
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

    BKLabel* typeLabel = BKCreator::create<BKLabel>();
    typeLabel->setText("脚本类型");
    BKCell* typeCell = BKCreator::create(BKAnchor::AnchorType::None);
    typeCell->append(typeLabel);
    BKComboBox* typeComboBox = BKCreator::create<BKComboBox>();
    typeComboBox->setItems({"", "glsl", "hlsl", "glsles", "metal", "glslvk", "hlslvk"});
    typeComboBox->setCurrentIndex(0, false);
    typeComboBox->setDataChangeCallback(std::bind(&SimpleMaterialCard::slotProgramTypeChanged, this, std::placeholders::_1));
    BKCell* typeComboBoxCell = BKCreator::create(BKAnchor::AnchorType::None);
    typeComboBoxCell->append(typeComboBox);

    BKLabel* autoParamLabel = BKCreator::create<BKLabel>();
    autoParamLabel->setText("自动参数");
    BKLabel* autoParamTitle1 = BKCreator::create<BKLabel>();
    autoParamTitle1->setText("名称");
    autoParamTitle1->setAlignment(Qt::AlignCenter);
    autoParamTitle1->setMinWidth(60);
    BKLabel* autoParamTitle2 = BKCreator::create<BKLabel>();
    autoParamTitle2->setText("绑定");
    autoParamTitle2->setAlignment(Qt::AlignCenter);
    autoParamTitle2->setMinWidth(140);
    BKCell* autoParamTitleCell = BKCreator::create(BKAnchor::AnchorType::None);
    autoParamTitleCell->append({ autoParamTitle1, autoParamTitle2 });

    BKCell* autoParamCell = BKCreator::create(BKAnchor::AnchorType::None);
    autoParamCell->append(autoParamLabel);
    BKCell* dynamicAutoParamCell = BKCreator::create(BKAnchor::AnchorType::None, BKCell::Type::ListGroup);
    BKLineEdit* autoParamBindEdit = new BKLineEdit();
    autoParamBindEdit->setMinWidth(60);
    QStringList paramList;
    paramList << "";
    for (const auto& item : name2AutoConstType)
        paramList << item.first.c_str();
    BKComboBox* autoParamComboBox = BKCreator::create<BKComboBox>();
    autoParamComboBox->setMinWidth(140);
    autoParamComboBox->setItems(paramList);
    autoParamComboBox->setCurrentIndex(0, false);
    dynamicAutoParamCell->setTemplate({ autoParamBindEdit, autoParamComboBox });
    dynamicAutoParamCell->setMemberCountChangedCallback(std::bind(&SimpleMaterialCard::slotAutoParamCountChanged, this, std::placeholders::_1, std::placeholders::_2));
    dynamicAutoParamCell->setMemberDataChangedCallback(std::bind(&SimpleMaterialCard::slotAutoParamDataChanged, this, std::placeholders::_1, std::placeholders::_2));


    BKLabel* constParamLabel = BKCreator::create<BKLabel>();
    constParamLabel->setText("常量参数");
    BKCell* constParamCell = BKCreator::create(BKAnchor::AnchorType::None);
    constParamCell->append(constParamLabel);
    BKLabel* constParamTitle1 = BKCreator::create<BKLabel>();
    constParamTitle1->setText("类型");
    constParamTitle1->setAlignment(Qt::AlignCenter);
    constParamTitle1->setMinWidth(60);
    BKLabel* constParamTitle2 = BKCreator::create<BKLabel>();
    constParamTitle2->setText("名称");
    constParamTitle2->setAlignment(Qt::AlignCenter);
    constParamTitle2->setMinWidth(60);
    BKLabel* constParamTitle3 = BKCreator::create<BKLabel>();
    constParamTitle3->setText("数值");
    constParamTitle3->setAlignment(Qt::AlignCenter);
    constParamTitle3->setMinWidth(130);
    BKCell* constParamTitleCell = BKCreator::create(BKAnchor::AnchorType::None);
    constParamTitleCell->append({ constParamTitle1, constParamTitle2, constParamTitle3});

    BKCell* dynamicConstParamCell = BKCreator::create(BKAnchor::AnchorType::None, BKCell::Type::ListGroup);
    QStringList types;
    types << "";
    for (const auto& item : name2ParamType)
        types << item.first.c_str();
    BKComboBox* constParamComboBox = BKCreator::create<BKComboBox>();
    constParamComboBox->setMinWidth(60);
    constParamComboBox->setItems(types);
    constParamComboBox->setCurrentIndex(0, false);
    BKLineEdit* constParamNameEdit = new BKLineEdit();
    constParamNameEdit->setMinWidth(60);
    BKLineEdit* constParamValueEdit = new BKLineEdit();
    constParamValueEdit->setMinWidth(130);
    dynamicConstParamCell->setTemplate({ constParamComboBox, constParamNameEdit, constParamValueEdit });
    dynamicConstParamCell->setMemberCountChangedCallback(std::bind(&SimpleMaterialCard::slotAutoParamCountChanged, this, std::placeholders::_1, std::placeholders::_2));
    dynamicConstParamCell->setMemberDataChangedCallback(std::bind(&SimpleMaterialCard::slotAutoParamDataChanged, this, std::placeholders::_1, std::placeholders::_2));
   
    BKLabel* vertexProgram = BKCreator::create<BKLabel>();
    vertexProgram->setText("顶点着色器程序");
    BKCell* vertexProgramCell = BKCreator::create(BKAnchor::AnchorType::Input);
    vertexProgramCell->setDataType(BKAnchor::Input, BKAnchor::String);
    vertexProgramCell->append(vertexProgram);

    BKLabel* fragmentProgram = BKCreator::create<BKLabel>();
    fragmentProgram->setText("片段着色器程序");
    BKCell* fragmentProgramProgramCell = BKCreator::create(BKAnchor::AnchorType::Input);
    fragmentProgramProgramCell->setDataType(BKAnchor::Input, BKAnchor::String);
    fragmentProgramProgramCell->append(fragmentProgram);

    _pack({
        outputCell,
        typeCell,
        typeComboBoxCell,
        autoParamCell,
        autoParamTitleCell,
        dynamicAutoParamCell,
        constParamCell,
        constParamTitleCell,
        dynamicConstParamCell,
        vertexProgramCell,
        fragmentProgramProgramCell,
        });

    // createMaterial2();
}


QVariant SimpleMaterialCard::getCurrentCardValue()
{
    return QString(mstrMaterialName.c_str());
}

#define JUST_WANNA_HIGHLIGHT(x) #x
void SimpleMaterialCard::createMaterial()
{
    Ogre::HighLevelGpuProgramPtr prog = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
        "JBMaterialTestVP", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
        "glsl", Ogre::GPT_VERTEX_PROGRAM);
    // prog->setSource(progSource);
    // prog->setVertexTextureFetchRequired(true);   // 在使用手动创建的Texture时使用吗？ e.g. TextureManager::getSingleton().createManual(something)
    
    {   // 似乎opengl的程序是需要.....的吧
        // prog->setParameter("target", "vp40");       
        // prog->setParameter("entry_point", "main");
    }
    
    prog->load();

    mstrMaterialName = "JBMaterialTest";
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(mstrMaterialName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
    pass->setVertexProgram("JBMaterialTestVP");
    Ogre::GpuProgramParametersSharedPtr vp = pass->getVertexProgramParameters();
    // vp->setNamedAutoConstant("world", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
    // vp->setNamedAutoConstant("viewProj", Ogre::GpuProgramParameters::ACT_VIEWPROJ_MATRIX);
    vp->setNamedAutoConstant("u_mvp", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    vp->setNamedAutoConstant("u_cam_pos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
    // vp->setNamedConstant("u_color", Ogre::Vector4(1.0f, 0,  0, 1.0f));

}

#if 0
#include <QFile>
void MaterialCard::createMaterial2()
{
    Ogre::String progSource;
    QFile script("C:\\Users\\Assaber\\Desktop\\standby\\Axis.material");
    script.open(QIODevice::ReadOnly);
    progSource = script.readAll().toStdString();
    script.close();


    Ogre::String vs = JUST_WANNA_HIGHLIGHT(#version 330 \n
        in vec3 vertex; \n
        uniform vec3 u_cam_pos; \n
        uniform mat4 u_mvp; \n
        void main(void) \n
        { \n
            float scale = length(u_cam_pos) * 0.01; \n
            gl_Position = u_mvp * vec4(vertex * scale, 1.0); \n
        } \n
        );

    Ogre::HighLevelGpuProgramPtr prog = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
        "axis_vs.glsl", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        "glsl", Ogre::GPT_VERTEX_PROGRAM);
    prog->setSource(vs);
    // prog->setVertexTextureFetchRequired(true);   // 在使用手动创建的Texture时使用吗？ e.g. TextureManager::getSingleton().createManual(something)

    {   // 似乎opengl的程序是需要.....的吧
        // prog->setParameter("target", "vp40");       
        // prog->setParameter("entry_point", "main");
    }
    prog->load();

    Ogre::String ps = JUST_WANNA_HIGHLIGHT(#version 330 \n
        out vec4 fragColour; \n
        uniform vec4 u_color; \n
        void main(void) \n
        { \n
            fragColour = u_color; \n
        });

    Ogre::HighLevelGpuProgramPtr prog2 = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
        "axis_ps.glsl", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        "glsl", Ogre::GPT_FRAGMENT_PROGRAM);
    prog2->setSource(ps);
    // prog->setVertexTextureFetchRequired(true);   // 在使用手动创建的Texture时使用吗？ e.g. TextureManager::getSingleton().createManual(something)

    {   // 似乎opengl的程序是需要.....的吧
        // prog->setParameter("target", "vp40");       
        // prog->setParameter("entry_point", "main");
    }

    prog2->load();


#if 0
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(mstrMaterialName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
    pass->setVertexProgram("axis_vs.glsl");
    pass->setFragmentProgram("axis_ps.glsl");

    Ogre::GpuProgramParametersSharedPtr vp = pass->getVertexProgramParameters();
    vp->setNamedAutoConstant("u_mvp", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    vp->setNamedAutoConstant("u_cam_pos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
    Ogre::GpuProgramParametersSharedPtr fp = pass->getFragmentProgramParameters();
    fp->setNamedConstant("u_color", Ogre::Vector4(1.0f, 0, 0, 1.0f));
#endif

    mstrMaterialName = "Assaber/Axis/X";


    Ogre::DataStreamPtr programPtr = Ogre::MemoryDataStreamPtr(OGRE_NEW Ogre::MemoryDataStream((void*)progSource.c_str(), progSource.size(), false, true));
    Ogre::ScriptCompilerManager::getSingletonPtr()->parseScript(programPtr, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
#endif

bool SimpleMaterialCard::slotProgramTypeChanged(const QVariant& type)
{
    return true;
}

void SimpleMaterialCard::slotAutoParamCountChanged(size_t count, const QVariantList& params)
{

}

void SimpleMaterialCard::slotAutoParamDataChanged(size_t count, const QVariantList& params)
{

}

void SimpleMaterialCard::recreateMaterial()
{

}