#pragma once
#include "container/BKCard.h"
#include "OgreGpuProgramParams.h"
#include "OgreString.h"

class BKAnchor;
class SimpleMaterialProgramCard : public BKCard
{
public:
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

    struct AutoConstantInfo
    {
        Ogre::String name = "";
        Ogre::GpuProgramParameters::AutoConstantType type = Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX;
    };

    struct ConstantInfo
    {
        ParamType type = ParamType::Real;
        Ogre::String name = "";
        Ogre::String value = "";
    };

public:
    SimpleMaterialProgramCard();
    
protected:
    virtual bool slotProgramTypeChanged(const QVariant& type) = 0;
    virtual void slotAutoConstantParamCountChanged(size_t count, const QVariantList& params) = 0;
    virtual void slotAutoConstantParamDataChanged(size_t count, const QVariantList& params) = 0;
    virtual void slotConstantParamCountChanged(size_t count, const QVariantList& params) = 0;
    virtual void slotConstantParamDataChanged(size_t count, const QVariantList& params) = 0;
    virtual bool slotProgramChanged(const QVariant& type) = 0;

protected:
    BKAnchor* mpOutputAnchor = nullptr;
};

class SimpleVertexProgCard : public SimpleMaterialProgramCard
{
    CREATE_CARD_FACTORY(SimpleVertexProgCard)

public:
    struct ProgramInfo
    {
        Ogre::String name = "";
        Ogre::String type = "";
        std::vector<AutoConstantInfo> aci;
        std::vector<ConstantInfo> ci;

        operator QVariant() const {
            return QVariant::fromValue(*this);
        }
    };

    SimpleVertexProgCard();
    virtual QVariant getCurrentCardValue() override;

protected:
    virtual bool slotProgramTypeChanged(const QVariant& type) override;
    virtual void slotAutoConstantParamCountChanged(size_t count, const QVariantList& params) override;
    virtual void slotAutoConstantParamDataChanged(size_t count, const QVariantList& params) override;
    virtual void slotConstantParamCountChanged(size_t count, const QVariantList& params) override;
    virtual void slotConstantParamDataChanged(size_t count, const QVariantList& params) override;
    virtual bool slotProgramChanged(const QVariant& data) override;

private: 
    bool createGpuProgram(bool recreate = true);

private:
    ProgramInfo mInfo;
    Ogre::String mProgram = "";
};
Q_DECLARE_METATYPE(SimpleVertexProgCard::ProgramInfo)

class SimpleFragmentProgCard : public SimpleMaterialProgramCard
{
    CREATE_CARD_FACTORY(SimpleFragmentProgCard)

public:
    struct ProgramInfo
    {
        Ogre::String name = "";
        Ogre::String type = "";
        std::vector<AutoConstantInfo> aci;
        std::vector<ConstantInfo> ci;

        operator QVariant() const {
            return QVariant::fromValue(*this);
        }
    };

    SimpleFragmentProgCard();
    virtual QVariant getCurrentCardValue() override;

protected:
    virtual bool slotProgramTypeChanged(const QVariant& type) override;
    virtual void slotAutoConstantParamCountChanged(size_t count, const QVariantList& params) override;
    virtual void slotAutoConstantParamDataChanged(size_t count, const QVariantList& params) override;
    virtual void slotConstantParamCountChanged(size_t count, const QVariantList& params) override;
    virtual void slotConstantParamDataChanged(size_t count, const QVariantList& params) override;
    virtual bool slotProgramChanged(const QVariant& data) override;

private:
    bool createGpuProgram(bool recreate = true);

private:
    ProgramInfo mInfo;
    Ogre::String mProgram = "";
};
Q_DECLARE_METATYPE(SimpleFragmentProgCard::ProgramInfo)