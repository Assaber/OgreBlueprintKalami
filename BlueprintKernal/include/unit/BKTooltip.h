#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

class _BlueprintKernalExport BKTooltip : public BKUnitBasic<BKTooltip>
{
public:
    using super = BKUnitBasic<BKTooltip>;
    BKTooltip(const QString& tooltip = "");
    ~BKTooltip();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    BKTooltip* setTooltip(const QString& text);

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void dataChanged(const QVariant& data) override;
};