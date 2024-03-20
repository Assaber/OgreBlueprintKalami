#include "unit/BKUnit.h"
#include <QDebug>

BKUnit::BKUnit()
{
}

BKUnit::~BKUnit()
{
    qDebug() << "Unit self delete";
}

BKCard* BKUnit::getBindCard() const
{
    return mpBindCard;
}

void BKUnit::resized() {}
