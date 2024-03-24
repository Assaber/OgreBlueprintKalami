#include "unit/BKUnit.h"
#include <QDebug>

BKUnit::BKUnit()
    : mCallbackFunc(BKUnit::defaultDataChangeCallback)
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

bool BKUnit::defaultDataChangeCallback(const QVariant& param)
{
    return false;
}

void BKUnit::resized() {}

void BKUnit::registOutputAnchor(BKAnchor* anchor)
{
    mpRightAnchor = anchor;
}
