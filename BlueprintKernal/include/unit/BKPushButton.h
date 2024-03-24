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
    /**
     * @brief:                                  设置显示文字
     * @param: const QString & text             文字内容
     * @param: bool comfy                       是否让按钮边界贴合文字
     * @return: BKPushButton*                   自身引用
     * @remark: 
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