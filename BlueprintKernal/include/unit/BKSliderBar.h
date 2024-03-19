#pragma once
#include "global_blueprint_kernal.h"
#include "BKUnit.h"

class _BlueprintKernalExport BKSliderBar : public BKUnitBasic<BKSliderBar>
{
public:
    enum class DateType : uint8_t
    {
        Int,
        Double,
    };

public:
    using super = BKUnitBasic<BKSliderBar>;
    BKSliderBar(DateType type = DateType::Int);
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
};