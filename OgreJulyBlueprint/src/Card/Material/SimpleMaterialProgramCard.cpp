#include "Material/SimpleMaterialProgramCard.h"

#include "BKCreator.hpp"
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

#include <QUuid>
#include <QDebug>

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

static std::map<Ogre::String, SimpleMaterialProgramCard::ParamType> name2ParamType = {
    {"Real", SimpleMaterialProgramCard::ParamType::Real},
    {"Int", SimpleMaterialProgramCard::ParamType::Int},
    {"UInt", SimpleMaterialProgramCard::ParamType::UInt},
    {"Bool", SimpleMaterialProgramCard::ParamType::Boolean},
    {"Vector4", SimpleMaterialProgramCard::ParamType::Vector4},
    {"Vector3", SimpleMaterialProgramCard::ParamType::Vector3},
    {"Vector2", SimpleMaterialProgramCard::ParamType::Vector2},
    {"Matrix3", SimpleMaterialProgramCard::ParamType::Matrix3},
    {"Matrix4", SimpleMaterialProgramCard::ParamType::Matrix4},
};

SimpleMaterialProgramCard::SimpleMaterialProgramCard()
{
    BKLabel* outputLabel = BKCreator::create<BKLabel>();
    outputLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    outputLabel->setText("Output");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, BKAnchor::String);
    outputCell->append(outputLabel, false);;
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

    BKLabel* typeLabel = BKCreator::create<BKLabel>();
    typeLabel->setText("Script type");
    BKCell* typeCell = BKCreator::create(BKAnchor::AnchorType::None);
    typeCell->append(typeLabel);
    BKComboBox* typeComboBox = BKCreator::create<BKComboBox>();
    typeComboBox->setItems({"", "glsl", "hlsl", "glsles", "metal", "glslvk", "hlslvk"});
    typeComboBox->setCurrentIndex(0, false);
    typeComboBox->setDataChangeCallback(std::bind(&SimpleMaterialProgramCard::slotProgramTypeChanged, this, std::placeholders::_1, std::placeholders::_2));
    BKCell* typeComboBoxCell = BKCreator::create(BKAnchor::AnchorType::None);
    typeComboBoxCell->append(typeComboBox);

    BKLabel* autoParamLabel = BKCreator::create<BKLabel>();
    autoParamLabel->setText("Auto const param");
    BKLabel* autoParamTitle1 = BKCreator::create<BKLabel>();
    autoParamTitle1->setText("Name");
    autoParamTitle1->setAlignment(Qt::AlignCenter);
    autoParamTitle1->setMinWidth(60);
    BKLabel* autoParamTitle2 = BKCreator::create<BKLabel>();
    autoParamTitle2->setText("Bind");
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
    dynamicAutoParamCell->setMemberCountChangedCallback(std::bind(&SimpleMaterialProgramCard::slotAutoConstantParamCountChanged, this, std::placeholders::_1, std::placeholders::_2));
    dynamicAutoParamCell->setMemberDataChangedCallback(std::bind(&SimpleMaterialProgramCard::slotAutoConstantParamDataChanged, this, std::placeholders::_1, std::placeholders::_2));


    BKLabel* constParamLabel = BKCreator::create<BKLabel>();
    constParamLabel->setText("Const param");
    BKCell* constParamCell = BKCreator::create(BKAnchor::AnchorType::None);
    constParamCell->append(constParamLabel);
    BKLabel* constParamTitle1 = BKCreator::create<BKLabel>();
    constParamTitle1->setText("Type");
    constParamTitle1->setAlignment(Qt::AlignCenter);
    constParamTitle1->setMinWidth(60);
    BKLabel* constParamTitle2 = BKCreator::create<BKLabel>();
    constParamTitle2->setText("Name");
    constParamTitle2->setAlignment(Qt::AlignCenter);
    constParamTitle2->setMinWidth(60);
    BKLabel* constParamTitle3 = BKCreator::create<BKLabel>();
    constParamTitle3->setText("Value");
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
    dynamicConstParamCell->setMemberCountChangedCallback(std::bind(&SimpleMaterialProgramCard::slotConstantParamCountChanged, this, std::placeholders::_1, std::placeholders::_2));
    dynamicConstParamCell->setMemberDataChangedCallback(std::bind(&SimpleMaterialProgramCard::slotConstantParamDataChanged, this, std::placeholders::_1, std::placeholders::_2));
   
    BKLabel* programLabel = BKCreator::create<BKLabel>();
    programLabel->setText("Program");
    programLabel->setTitleNeverChanges(true);
    programLabel->setDataChangeCallback(std::bind(&SimpleMaterialProgramCard::slotProgramChanged, this, std::placeholders::_1, std::placeholders::_2));
    BKCell* programCell = BKCreator::create(BKAnchor::AnchorType::Input);
    programCell->setDataType(BKAnchor::AnchorType::Input, BKAnchor::String);
    programCell->append(programLabel);

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
        programCell
        });
}

SimpleVertexProgCard::SimpleVertexProgCard()
    : SimpleMaterialProgramCard()
{
    setTitle("Simple vertex program");
    mpOutputAnchor->setDateType(QMetaTypeId<ProgramInfo>::qt_metatype_id());
}

QVariant SimpleVertexProgCard::getCurrentCardValue()
{
    return mInfo;
}

bool SimpleVertexProgCard::slotProgramTypeChanged(BKUnit* unit, const QVariant& type)
{
    QString qType = type.toString();
    mInfo.type = qType.toStdString();
    createGpuProgram();
    return true;
}

void SimpleVertexProgCard::slotAutoConstantParamCountChanged(size_t count, const QVariantList& params)
{
    auto& vec = mInfo.aci;
    vec.clear();
    vec.reserve(count / 2);

    for (QVariantList::const_iterator itor = params.begin(); itor != params.end(); )
    {
        Ogre::String bindName = itor->toString().toStdString();
        ++itor;
        Ogre::String paramType = itor->toString().toStdString();
        ++itor;

        if (bindName.size() && paramType.size()) {
            vec.push_back({ bindName, name2AutoConstType[paramType] });
        }
    }

    createGpuProgram(false);
}

void SimpleVertexProgCard::slotAutoConstantParamDataChanged(size_t count, const QVariantList& params)
{
    slotAutoConstantParamCountChanged(count, params);
}

void SimpleVertexProgCard::slotConstantParamCountChanged(size_t count, const QVariantList& params)
{
    auto& vec = mInfo.ci;
    vec.clear();
    vec.reserve(count / 3);

    for (QVariantList::const_iterator itor = params.begin(); itor != params.end(); )
    {
        Ogre::String type = itor->toString().toStdString();
        ++itor;
        Ogre::String name = itor->toString().toStdString();
        ++itor;
        Ogre::String value = itor->toString().toStdString();
        ++itor;

        if (type.size() && name.size() && value.size()) {
            vec.push_back({ name2ParamType[type], name, value });
        }
    }

    createGpuProgram(false);
}

void SimpleVertexProgCard::slotConstantParamDataChanged(size_t count, const QVariantList& params)
{
    slotConstantParamCountChanged(count, params);
}

bool SimpleVertexProgCard::slotProgramChanged(BKUnit* unit, const QVariant& data)
{
    QString program = data.toString();
    mProgram = program.toStdString();
    createGpuProgram(true);

    return true;
}

bool SimpleVertexProgCard::createGpuProgram(bool recreate)
{
    bool skip = true;
    Ogre::String oldProgramName = mInfo.name;
    Ogre::HighLevelGpuProgramManager* hlpm = Ogre::HighLevelGpuProgramManager::getSingletonPtr();

    do 
    {
        if (mInfo.type.size() == 0 || mProgram.size() == 0)
            break;

        if (!recreate && !mInfo.name.empty())
        {
            Ogre::HighLevelGpuProgramPtr pp = hlpm->getByName(mInfo.name);
            if (pp)
            {
                skip = false;
                break;
            }
        }

        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        mInfo.name = uuid.toStdString() + "_vp";

        Ogre::HighLevelGpuProgramPtr programPtr = hlpm->createProgram(
            mInfo.name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            mInfo.type, Ogre::GPT_VERTEX_PROGRAM);

        programPtr->setSource(mProgram);
        // programPtr->setVertexTextureFetchRequired(true);   // 在使用手动创建的Texture时使用吗？ e.g. TextureManager::getSingleton().createManual(something)
        programPtr->load();

        skip = false;

    } while (false);

    if (skip)
    {
        qWarning() << "Gpu顶点程序名称回滚";
        mInfo.name = oldProgramName;
    }
    else
    {
        mpOutputAnchor->dataChanged(mInfo);

        if (recreate && !oldProgramName.empty()) {
            // 释放旧的材质
            Ogre::HighLevelGpuProgramManager* hlpm = Ogre::HighLevelGpuProgramManager::getSingletonPtr();
            Ogre::HighLevelGpuProgramPtr programPtr = hlpm->getByName(oldProgramName);
            if (programPtr)
            {
                programPtr->unload();
                hlpm->remove(programPtr);
            }
        }
    }
   
    return !skip;
}

SimpleFragmentProgCard::SimpleFragmentProgCard()
{
    setTitle("Simple fragment program");
    mpOutputAnchor->setDateType(QMetaTypeId<ProgramInfo>::qt_metatype_id());
}

QVariant SimpleFragmentProgCard::getCurrentCardValue()
{
    return mInfo;
}

bool SimpleFragmentProgCard::slotProgramTypeChanged(BKUnit* unit, const QVariant& type)
{
    QString qType = type.toString();
    mInfo.type = qType.toStdString();
    createGpuProgram();
    return true;
}

void SimpleFragmentProgCard::slotAutoConstantParamCountChanged(size_t count, const QVariantList& params)
{
    auto& vec = mInfo.aci;
    vec.clear();
    vec.reserve(count / 2);

    for (QVariantList::const_iterator itor = params.begin(); itor != params.end(); )
    {
        Ogre::String bindName = itor->toString().toStdString();
        ++itor;
        Ogre::String paramType = itor->toString().toStdString();
        ++itor;

        if (bindName.size() && paramType.size()) {
            vec.push_back({ bindName, name2AutoConstType[paramType] });
        }
    }

    createGpuProgram(false);
}

void SimpleFragmentProgCard::slotAutoConstantParamDataChanged(size_t count, const QVariantList& params)
{
    slotAutoConstantParamCountChanged(count, params);
}

void SimpleFragmentProgCard::slotConstantParamCountChanged(size_t count, const QVariantList& params)
{
    auto& vec = mInfo.ci;
    vec.clear();
    vec.reserve(count / 3);

    for (QVariantList::const_iterator itor = params.begin(); itor != params.end(); )
    {
        Ogre::String type = itor->toString().toStdString();
        ++itor;
        Ogre::String name = itor->toString().toStdString();
        ++itor;
        Ogre::String value = itor->toString().toStdString();
        ++itor;

        if (type.size() && name.size() && value.size()) {
            vec.push_back({ name2ParamType[type], name, value });
        }
    }
    createGpuProgram(false);
}

void SimpleFragmentProgCard::slotConstantParamDataChanged(size_t count, const QVariantList& params)
{
    slotConstantParamCountChanged(count, params);
}

bool SimpleFragmentProgCard::slotProgramChanged(BKUnit* unit, const QVariant& data)
{
    QString qData = data.toString();
    mProgram = qData.toStdString();
    createGpuProgram();

    return true;
}

bool SimpleFragmentProgCard::createGpuProgram(bool recreate /*= true*/)
{
    bool skip = true;
    Ogre::String oldProgramName = mInfo.name;
    Ogre::HighLevelGpuProgramManager* hlpm = Ogre::HighLevelGpuProgramManager::getSingletonPtr();

    do
    {
        if (mInfo.type.size() == 0 || mProgram.size() == 0)
            break;

        if (!recreate && !mInfo.name.empty())
        {
            Ogre::HighLevelGpuProgramPtr pp = hlpm->getByName(mInfo.name);
            if (pp)
            {
                skip = false;
                break;
            }
        }

        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        mInfo.name = uuid.toStdString() + "_fp";

        Ogre::HighLevelGpuProgramPtr programPtr = hlpm->createProgram(
            mInfo.name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            mInfo.type, Ogre::GPT_FRAGMENT_PROGRAM);

        programPtr->setSource(mProgram);
        // programPtr->setVertexTextureFetchRequired(true);   // 在使用手动创建的Texture时使用吗？ e.g. TextureManager::getSingleton().createManual(something)
        programPtr->load();

        skip = false;

    } while (false);

    if (skip)
    {
        qWarning() << "Gpu片段程序名称回滚";
        mInfo.name = oldProgramName;
    }
    else
    {
        mpOutputAnchor->dataChanged(mInfo);

        if (recreate && !oldProgramName.empty()) {
            // 释放旧的材质
            Ogre::HighLevelGpuProgramManager* hlpm = Ogre::HighLevelGpuProgramManager::getSingletonPtr();
            Ogre::HighLevelGpuProgramPtr programPtr = hlpm->getByName(oldProgramName);
            if (programPtr)
            {
                programPtr->unload();
                hlpm->remove(programPtr);
            }
        }
    }

    return !skip;
}
