#pragma once
#include "global_blueprint_kernal.h"
#include "BKUnit.h"
#include <QGraphicsObject>
#include <QSize>

class _BlueprintKernalExport BKComboBox : public BKUnitBasic<BKComboBox>
{
public:
    using super = BKUnitBasic<BKComboBox>;
    BKComboBox();
    ~BKComboBox();

public:
    BKComboBox* setDefaultIndex(int index);

    BKComboBox* setItems(const QStringList& items);
    
private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
};