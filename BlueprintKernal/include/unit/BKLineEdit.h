#pragma once
#include "global_blueprint_kernal.h"
#include "BKUnit.h"

class _BlueprintKernalExport BKLineEdit : public BKUnitBasic<BKLineEdit>
{
public:
    using super = BKUnitBasic<BKLineEdit>;
    BKLineEdit();
    ~BKLineEdit();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    BKLineEdit* setText(const QString& text);
    BKLineEdit* setEditable(bool enable);

public:
    QString getCurrentText();
    
private:
    friend class BKLineEditor;
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void resized() override;
    virtual void dataChanged(const QVariant& data) override;

};