#pragma once
#include <stdint.h>
#include <QGraphicsItem>
#include "global_blueprint_kernal.h"

class _BlueprintKernalExport StandAloneUnit
{
public:
    enum class Type : uint16_t
    {
        Default,
        Card,
        ConnectingLine,
        PreConnectingLine,
    };

public:
    StandAloneUnit(Type type);
    virtual ~StandAloneUnit();

public:
    void syncSoulImprint();

public:
    inline Type getUnitType() const {
        return mUnitType;
    }

protected:
    virtual QGraphicsItem* getBindItem() = 0;

private:
    friend class BlueprintLoader;
    friend class BKAnchor;
    Type mUnitType = Type::Default;
};

