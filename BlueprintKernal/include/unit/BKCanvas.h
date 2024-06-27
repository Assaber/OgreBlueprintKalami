#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

class _BlueprintKernalExport BKCanvas : public BKUnitBasic<BKCanvas>
{
public:
    enum class BackgroundSetting : uint8_t
    {
        FillWhite,
        Transparent,

        Count,
    };

public:
    using super = BKUnitBasic<BKCanvas>;
    BKCanvas();
    ~BKCanvas();

public:
    BKCanvas* setBackgroundType(BackgroundSetting setting);

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void dataChanged(const QVariant& data) override;
    virtual void resized() override;
};