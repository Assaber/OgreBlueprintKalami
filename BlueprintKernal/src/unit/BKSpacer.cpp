#include "unit/BKSpacer.h"

class BKSpacer::Impl : public QGraphicsItem
{
public:
    Impl(BKSpacer* handle) : mpHandle(handle) {}

public:
    BKSpacer* mpHandle = nullptr;
   

public:
    QRectF boundingRect() const override
    {
        return { 0, 0, mpHandle->mSize.width(), mpHandle->mSize.height() };
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static_cast<void*>(widget);
        static_cast<void*>(painter);
    }
};

BKSpacer::BKSpacer()
    : super()
    , mpImpl(new Impl(this))
{
    setSizePolicy(SizePolicy::Adaptive);
}

BKSpacer::~BKSpacer()
{
    delete mpImpl;
    mpImpl = nullptr;
}

QJsonValue BKSpacer::getValue()
{
    return 0;
}

bool BKSpacer::setValue(const QJsonValue& val)
{
    return true;
}

QGraphicsItem* BKSpacer::getGraphicsItem()
{
    return mpImpl;
}

void BKSpacer::dataChanged(const QVariant& data)
{
    return;
}
