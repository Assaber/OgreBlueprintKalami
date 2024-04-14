#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <QString>

/*
    这里我不是很想让BKLabel用在锚点中间去作为一个可修改的控件来使用，因为他本身是不提供编辑功能的
    如果希望使用编辑功能的Label应该去选择BKLineEdit，而不是BKLabel
    如果是占位应该使用BKSpacer
    原规划的Label就是作为一个标题使用，这样其实也不用考虑getValue和setValue了（毕竟是相对固定的，且在创建卡片时就确定下来了）
    但是目前还是补齐他的Value get/set方法吧，虽然导入导出的时候会感觉臃肿，甚至会做无用的事情
    暂且这样，容我纠结一段时间
    todo...   24.03.31 --assaber
*/

class _BlueprintKernalExport BKLabel : public BKUnitBasic<BKLabel>
{
public:
    using super = BKUnitBasic<BKLabel>;
    BKLabel();
    ~BKLabel();

public:
    virtual QJsonValue getValue() override;
    virtual bool setValue(const QJsonValue& val) override;

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