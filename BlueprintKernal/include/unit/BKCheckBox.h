#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

class _BlueprintKernalExport BKCheckBox : public BKUnitBasic<BKCheckBox>
{
public:
    using super = BKUnitBasic<BKCheckBox>;
    BKCheckBox();
    ~BKCheckBox();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:

    /**
     * @brief:                          设置开关使能
     * @param: bool enable              使能
     * @return: BKCheckBox*             自身指针
     * @remark: 
     */
    BKCheckBox* setEnable(bool enable);

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;
};