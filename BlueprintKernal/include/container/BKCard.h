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
    virtual QGraphicsItem* getBindItem() override;

protected:
    void pack(std::initializer_list<BKCell*> cells);
   
private:
    class Impl;
    Impl* mpImpl = nullptr;
};

