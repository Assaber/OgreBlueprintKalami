﻿#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"

class _BlueprintKernalExport BKSpacer : public BKUnitBasic<BKSpacer>
{
public:
    using super = BKUnitBasic<BKSpacer>;
    BKSpacer();
    ~BKSpacer();

private:
    class Impl;
    Impl* mpImpl = nullptr;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual void dataChanged(const QVariant& data) override;
};