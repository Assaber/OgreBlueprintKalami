#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

class _BlueprintKernalExport BKPushButton : public BKUnitBasic<BKPushButton>
{
public:
    using super = BKUnitBasic<BKPushButton>;
    BKPushButton();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    /**
     * @param: bool comfy     Whether or not to make the button borders fit the text
     */
    BKPushButton* setText(const QString& text, bool comfy = false);

protected:
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;

private:
    class Impl;
    Impl* mpImpl;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;

};