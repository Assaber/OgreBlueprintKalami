#include "unit/BKTooltip.h"
#include <QPainter>
#include <QDebug>
#include "container/BKAnchor.h"

class BKTooltip::Impl : public QGraphicsItem
{
public:
    Impl(BKTooltip* handle) : mpHandle(handle) {}

public:
    BKTooltip* mpHandle = nullptr;
    QString mstrTooltip = "";

public:
    QRectF boundingRect() const override {
        return { 0, 0, mpHandle->mSize.width(), mpHandle->mSize.height() };
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static_cast<void*>(widget);

        painter->save();
        {
            painter->drawText(boundingRect(), Qt::AlignCenter, "(?)");
            painter->restore();
        }
    }
};

BKTooltip::BKTooltip(const QString& tooltip/* = ""*/)
    : super()
    , mpImpl(new Impl(this))
{
    setTooltip(tooltip);
    setFixedSize({ BKUnit::minUnitHeight, BKUnit::minUnitHeight });
}

BKTooltip::~BKTooltip()
{
    delete mpImpl;
    mpImpl = nullptr;
}

bool BKTooltip::loadFromJson(const QJsonValue& val)
{
    return true;
}

QVariant BKTooltip::data()
{
    return "";
}

BKTooltip::operator QJsonValue() const
{
    return "";
}

BKTooltip* BKTooltip::setTooltip(const QString& text)
{
    L_IMPL(BKTooltip);
    l->setToolTip(text);

    return this;
}

BKUnit* BKTooltip::copy()
{
    L_IMPL(BKTooltip);
    BKTooltip* target = BKCreator::create<BKTooltip>();

    BKTooltip::Impl* dstImpl = target->mpImpl;
    dstImpl->mstrTooltip = l->mstrTooltip;
    return target;
}

QGraphicsItem* BKTooltip::getGraphicsItem()
{
    return mpImpl;
}

void BKTooltip::dataChanged(const QVariant& data)
{
    // 我们坐下来聊，告诉我为什么要change
    return;
}
