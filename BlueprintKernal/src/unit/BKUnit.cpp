#include "unit/BKUnit.h"
#include "unit/BKUnitCreator.h"
#include <QDebug>

BKUnit::BKUnit()
{
}

BKUnit::~BKUnit()
{
    qDebug() << "Unit self delete";
}
