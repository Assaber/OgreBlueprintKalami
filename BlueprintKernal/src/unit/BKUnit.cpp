#include "unit/BKUnit.h"
#include "container/BKAnchor.h"

BKUnit::BKUnit()
    : mCallbackFunc(BKUnit::defaultDataChangeCallback) 
{
}

BKUnit::~BKUnit()
{
}

BKCard* BKUnit::getBindCard() const
{
    return mpBindCard;
}

bool BKUnit::defaultDataChangeCallback(BKUnit* unlit, const QVariant& param)
{
    return false;
}

void BKUnit::dataChanged()
{
    if (!mCallbackFunc(this, QVariant()) && mpRightAnchor)
        mpRightAnchor->dataChanged(data());
}

void BKUnit::resized() {}

void BKUnit::_copyBasicAttributeTo(BKUnit* dst)
{
    dst->mSize = this->mSize;
    dst->mMinSize = this->mMinSize;
    dst->mSizePolicy = this->mSizePolicy;
    dst->mpBindCard = this->mpBindCard;

    // The specific unit attribute needs to be manually bound internally
    // ...
}