#pragma once
#include "global_blueprint_kernal.h"
#include "BKUnit.h"
#include <QGraphicsObject>
#include <QSize>

class BKComboBoxItemView;
class _BlueprintKernalExport BKComboBox : public BKUnitBasic<BKComboBox>
{
public:
    enum CallbackParamType
    {
        Data,
        Index,
    };

public:
    using super = BKUnitBasic<BKComboBox>;
    BKComboBox();
    ~BKComboBox();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    // Fixed returns to current text(QString)
    // If current index < 0, it will return ""
    // It is not affected by CallbackParamType
    virtual QVariant data() override;
    virtual operator QJsonValue() const;

public:
    BKComboBox* setCurrentIndex(int index, bool notify = true);
    BKComboBox* setCurrentItem(const QString& item, bool notify = true);
    BKComboBox* setItems(const QStringList& items);
    BKComboBox* setCallbackParamType(CallbackParamType type);
    
private:
    friend class BKComboBoxItemView;
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;
};