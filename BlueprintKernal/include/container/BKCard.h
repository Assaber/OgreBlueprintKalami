#pragma once
#include "global_blueprint_kernal.h"
#include "container/StandAloneUnit.h"
#include "container/BKCell.h"
#include "BlueprintLoader.h"

class _BlueprintKernalExport BKCard : public StandAloneUnit
{
public:
    BKCard();
    virtual ~BKCard();

public:
    void setTitle(const QString& title);

protected:
    void pack(std::initializer_list<BKCell*> cells);
    virtual QGraphicsItem* getBindItem() override;

private:
    class Impl;
    Impl* mpImpl = nullptr;
};

