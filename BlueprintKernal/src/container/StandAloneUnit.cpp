#include "container/StandAloneUnit.h"


StandAloneUnit::StandAloneUnit(Type type)
    : mUnitType(type)
{

}

StandAloneUnit::~StandAloneUnit()
{

}

void StandAloneUnit::syncSoulImprint()
{
    QGraphicsItem* item = getBindItem();
    if (item)
        item->setData(StandAloneUnitInUserData, GET_ENUM_CLASS_UPWARD_VARIANT(mUnitType));
}