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
     * @return: BKLabel*                                    自身引用
     * @remark: 
     */
    BKLabel* setText(const QString& text);
    /**
     * @brief:                                              设置显示文字永久不发生改变
     * @param: bool enable                                  使能
     * @return: BKLabel*
     * @remark:                                             当设置为true时，锚点传入的数据将不会影响标题显示;原则上用于单锚点接收; 默认为false
     */
    BKLabel* setTitleNeverChanges(bool enable);
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