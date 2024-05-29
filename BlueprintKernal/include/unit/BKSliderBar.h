#pragma once
#include "global_blueprint_kernal.h"
#include "BKUnit.h"

class _BlueprintKernalExport BKSliderBar : public BKUnitBasic<BKSliderBar>
{
public:
    enum class DataType : uint8_t
    {
        Int,
        Double,
    };

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    using super = BKUnitBasic<BKSliderBar>;
    BKSliderBar(DataType type = DataType::Int);
    ~BKSliderBar();

public:
    BKSliderBar* setCurrentValue(const QVariant& value);
    BKSliderBar* setMaximum(const QVariant& max);
    BKSliderBar* setMinimum(const QVariant& min);
   

private:
    friend class BKSliderBarEditor;
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;

};