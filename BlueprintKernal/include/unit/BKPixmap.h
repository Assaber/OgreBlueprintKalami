#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"

class _BlueprintKernalExport BKPixmap : public BKUnitBasic<BKPixmap>
{
public:
    using super = BKUnitBasic<BKPixmap>;
    BKPixmap();
    ~BKPixmap();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    BKPixmap* setSource(const QString& path);

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void dataChanged(const QVariant& data) override;
    virtual void resized() override;
};