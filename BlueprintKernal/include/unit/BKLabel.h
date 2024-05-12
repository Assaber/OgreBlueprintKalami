#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

class _BlueprintKernalExport BKLabel : public BKUnitBasic<BKLabel>
{
public:
    using super = BKUnitBasic<BKLabel>;
    BKLabel();
    ~BKLabel();

public:
    virtual BKUnit* copy() override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

public:
    /**
     * @brief:                                              设置显示文字
     * @param: const QString & text                         文字内容
     * @param: bool dontUpdate                              是否在输入节点更新时不进行更新，默认为false
     * @return: BKLabel*                                    自身引用
     * @remark: 
     */
    BKLabel* setText(const QString& text, bool dontUpdate = false);
    /**
     * @brief:                                              设置对齐
     * @param: Qt::Alignment alignment                      Qt自带对齐
     * @return: BKLabel&                                    自身引用
     * @remark: 
     */
    BKLabel* setAlignment(Qt::Alignment alignment);

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void dataChanged(const QVariant& data) override;
};