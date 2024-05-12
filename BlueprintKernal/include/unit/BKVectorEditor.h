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
    // std::vector<int> 或std::vector<float>
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    BKVectorEditor* setNames(const std::vector<QString>& names);
    BKVectorEditor* setValue(const QVariant& value);
    /**
     * @brief: 
     * @param: int index                            
     * @param: const std::pair<QVariant
     * @param: QVariant> & minmax
     * @return: BKVectorEditor*
     * @remark:                                     index传入负数代表所有向量都应用这个范围
     */
    BKVectorEditor* setRange(int index, const std::pair<QVariant, QVariant>& minmax);
    /**
     * @brief: 
     * @param: int count
     * @return: BKVectorEditor*
     * @remark:                                     在生成卡片之前调用有效，否则可能会造成控件挤压
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