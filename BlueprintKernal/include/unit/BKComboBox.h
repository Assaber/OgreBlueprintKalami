#pragma once
#include "global_blueprint_kernal.h"
#include "BKUnit.h"
#include <QGraphicsObject>
#include <QSize>

class BKComboBoxItemView;
class _BlueprintKernalExport BKComboBox : public BKUnitBasic<BKComboBox>
{
public:
    using super = BKUnitBasic<BKComboBox>;
    BKComboBox();
    ~BKComboBox();


public:
    virtual QJsonValue getValue() override;
    virtual bool setValue(const QJsonValue& val) override;

public:
    BKComboBox* setCurrentIndex(int index, bool notify = true);
    BKComboBox* setCurrentItem(const QString& item, bool notify = true);
    BKComboBox* setItems(const QStringList& items);
    
private:
    friend class BKComboBoxItemView;
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;
};