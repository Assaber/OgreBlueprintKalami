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

void BKUnit::_copyBasicAttributeTo(BKUnit* dst)
{
    dst->mSize = this->mSize;
    dst->mMinSize = this->mMinSize;
    dst->mSizePolicy = this->mSizePolicy;
    dst->mpBindCard = this->mpBindCard;

    // 绑定相关的需要重新绑定
}