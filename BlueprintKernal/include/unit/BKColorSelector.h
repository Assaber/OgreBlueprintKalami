#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>
#include <array>

class _BlueprintKernalExport BKColorSelector : public BKUnitBasic<BKColorSelector>
{
public:
    using super = BKUnitBasic<BKColorSelector>;
    using Color4f = std::array<float, 4>;     // rgba
    using Color3f = std::array<float, 3>;     // rgb a=1

    enum class Type : uint8_t {
        Vector3,
        Vector4,
    };

public:
    BKColorSelector(Type type = Type::Vector3);
    ~BKColorSelector();

public:
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual operator QJsonValue() const override;
    // Returns a QVariant from QColor
    virtual QVariant data() override;
    virtual BKUnit* copy() override;

public:
    BKColorSelector* setColor(const Color4f& color);
    BKColorSelector* setColor(const Color3f& color);
    BKColorSelector* setColor(const QColor& color);

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;

public:
    static constexpr int fixedWidth = 35;
};