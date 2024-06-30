#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

class _BlueprintKernalExport BKLabel : public BKUnitBasic<BKLabel>
{
public:
    using super = BKUnitBasic<BKLabel>;
    BKLabel(const QString& text = "");
    ~BKLabel();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    /**
     * @remark: After setting the text, the minimum width of the widget will be changed, and the change rule will be based on the minimum width of the text.
     * So if you need to set the minimum width, you need to refresh it again after setting the text                                            
     */
    BKLabel* setText(const QString& text);
    /**
     * @remark:  When set to true, the data passed in from the anchor will not affect the title display; In principle, it is used for single anchor reception
     */
    BKLabel* setTitleNeverChanges(bool enable);

    BKLabel* setAlignment(Qt::Alignment alignment);

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void dataChanged(const QVariant& data) override;
};