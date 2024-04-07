#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>
#include <array>

class _BlueprintKernalExport BKColorSelectorEx : public BKUnitBasic<BKColorSelectorEx>
{
public:
    using super = BKUnitBasic<BKColorSelectorEx>;
    using Color4f = std::array<float, 4>;     // rgba
    using Color3f = std::array<float, 3>;     // rgb a=1

public:
    BKColorSelectorEx();
    ~BKColorSelectorEx();

public:
    virtual QJsonValue getValue() override;
    virtual bool setValue(const QJsonValue& val) override;

public:
    BKColorSelectorEx* setColor(const Color4f& color);
    BKColorSelectorEx* setColor(const Color3f& color);
    BKColorSelectorEx* setColor(const QColor& color);

private:
    friend class BKColorEditor;
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;
};