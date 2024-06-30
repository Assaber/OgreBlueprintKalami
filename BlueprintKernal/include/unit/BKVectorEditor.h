#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

class _BlueprintKernalExport BKVectorEditor : public BKUnitBasic<BKVectorEditor>
{
public:
    using IntegerVec = std::vector<int>;
    using FloatVec = std::vector<float>;

public:
    enum class Type : uint8_t
    {
        Float,
        Int,
    };

public:
    using super = BKUnitBasic<BKVectorEditor>;

    BKVectorEditor(Type type = Type::Float, uint32_t count = 3);
    ~BKVectorEditor();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    // std::vector<int> or std::vector<float>
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    /**
     * @remark: Six names are deposited by default(x\y\z\u\v\w)
     */
    BKVectorEditor* setNames(const std::vector<QString>& names);
    BKVectorEditor* setValue(const QVariant& value);
    /**
     * @remark: If Index is negative, all members will be constrained to this range
     */
    BKVectorEditor* setRange(int index, const std::pair<QVariant, QVariant>& minmax);
    /**
     * @remark: This function is valid before it's card is generated, otherwise it may cause the control to squeeze                                    
     */
    BKVectorEditor* setItemInLine(int count);

private:
    friend class BKVectorCtrl;
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;
};