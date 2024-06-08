#include "Material/SimpleMaterialCard.h"

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
#include "Material/SimpleMaterialProgramCard.h"
#include <QUuid>
#include <QDebug>

#include "OgreScriptCompiler.h"

SimpleMaterialCard::SimpleMaterialCard()
{
    setTitle("简单材质");
    mstrMaterialName = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();

    BKLabel* outputLabel = BKCreator::create<BKLabel>();
    outputLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    outputLabel->setText("输出");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, BKAnchor::String);
    outputCell->append(outputLabel, false);;
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();

    BKLabel* nameLabel = BKCreator::create<BKLabel>();
    nameLabel->setText("脚本类型");
    BKCell* nameCell = BKCreator::create(BKAnchor::AnchorType::None);
    nameCell->append(nameLabel);

    BKLineEdit* nameEdit = BKCreator::create<BKLineEdit>();
    nameEdit->setEnable(false);
    BKCell* nameEditCell = BKCreator::create(BKAnchor::AnchorType::None);
    nameEditCell->append(nameEdit);
    
    BKLabel* vertexProgram = BKCreator::create<BKLabel>();
    vertexProgram->setText("顶点着色器程序");
    vertexProgram->setTitleNeverChanges(true);
    vertexProgram->setDataChangeCallback(std::bind(&SimpleMaterialCard::slotVertexProgramUpdate, this, std::placeholders::_1, std::placeholders::_2));
    BKCell* vertexProgramCell = BKCreator::create(BKAnchor::AnchorType::Input);
    vertexProgramCell->setDataType(BKAnchor::Input, QMetaTypeId<SimpleVertexProgCard::ProgramInfo>::qt_metatype_id());
    vertexProgramCell->append(vertexProgram);

    BKLabel* fragmentProgram = BKCreator::create<BKLabel>();
    fragmentProgram->setText("片段着色器程序");
    fragmentProgram->setTitleNeverChanges(true);
    fragmentProgram->setDataChangeCallback(std::bind(&SimpleMaterialCard::slotFragmentProgramUpdate, this, std::placeholders::_1, std::placeholders::_2));
    BKCell* fragmentProgramProgramCell = BKCreator::create(BKAnchor::AnchorType::Input);
    fragmentProgramProgramCell->setDataType(BKAnchor::Input, QMetaTypeId<SimpleFragmentProgCard::ProgramInfo>::qt_metatype_id());
    fragmentProgramProgramCell->append(fragmentProgram);

    _pack({
        outputCell,
        nameCell,
        nameEditCell,
        vertexProgramCell,
        fragmentProgramProgramCell,
        });

    updateMaterial();
}


QVariant SimpleMaterialCard::getCurrentCardValue()
{
    return QString(mstrMaterialName.c_str());
}

void SimpleMaterialCard::updateMaterial()
{
    
    Ogre::MaterialManager* mm = Ogre::MaterialManager::getSingletonPtr();
    Ogre::MaterialPtr materialPtr = mm->getByName(mstrMaterialName);
    if (!materialPtr)
    {
        materialPtr = mm->create(mstrMaterialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    Ogre::Technique* technique = materialPtr->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    if (mVertexProgramInfo.name.size())
    {
        pass->setVertexProgram(mVertexProgramInfo.name);

        Ogre::GpuProgramParametersSharedPtr vpp = pass->getVertexProgramParameters();

        for (const auto& item : mVertexProgramInfo.aci)
            vpp->setNamedAutoConstant(item.name, item.type);

        for (const auto& item : mVertexProgramInfo.ci)
            transConstantInfo2Program(item, vpp);
    }

    if (mFragmentProgramInfo.name.size())
    {
        pass->setFragmentProgram(mFragmentProgramInfo.name);

        Ogre::GpuProgramParametersSharedPtr fpp = pass->getFragmentProgramParameters();

        for (const auto& item : mFragmentProgramInfo.aci)
            fpp->setNamedAutoConstant(item.name, item.type);

        for (const auto& item : mFragmentProgramInfo.ci)
            transConstantInfo2Program(item, fpp);
    }

    mpOutputAnchor->dataChanged(getCurrentCardValue());
}

bool SimpleMaterialCard::slotVertexProgramUpdate(BKUnit* unit, const QVariant& vd)
{
    if (!vd.canConvert<SimpleVertexProgCard::ProgramInfo>())
        return true;

    mVertexProgramInfo = vd.value<SimpleVertexProgCard::ProgramInfo>();

    updateMaterial();
    return true;
}

bool SimpleMaterialCard::slotFragmentProgramUpdate(BKUnit* unit, const QVariant& fd)
{
    if (!fd.canConvert<SimpleFragmentProgCard::ProgramInfo>())
        return true;

    mFragmentProgramInfo = fd.value<SimpleFragmentProgCard::ProgramInfo>();

    updateMaterial();
    return true;
}

void SimpleMaterialCard::transConstantInfo2Program(const SimpleMaterialProgramCard::ConstantInfo& src, Ogre::GpuProgramParametersSharedPtr dst)
{
    switch (src.type)
    {
    case SimpleMaterialProgramCard::Real:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseReal(src.value));
        break;
    case SimpleMaterialProgramCard::Int:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseInt(src.value));
        break;
    case SimpleMaterialProgramCard::UInt:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseUnsignedInt(src.value));
        break;
    case SimpleMaterialProgramCard::Boolean:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseBool(src.value));
        break;
    case SimpleMaterialProgramCard::Vector4:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseVector4(src.value));
        break;
    case SimpleMaterialProgramCard::Vector3:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseVector3(src.value));
        break;
    case SimpleMaterialProgramCard::Vector2:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseVector2(src.value));
        break;
    case SimpleMaterialProgramCard::Matrix3:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseMatrix3(src.value));
        break;
    case SimpleMaterialProgramCard::Matrix4:
        dst->setNamedConstant(src.name, Ogre::StringConverter::parseMatrix4(src.value));
        break;
    default:
        break;
    }
}