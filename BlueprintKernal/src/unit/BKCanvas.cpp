#include "unit/BKCanvas.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include "container/BKAnchor.h"

class BKCanvas::Impl : public QGraphicsItem
{
public:
    Impl(BKCanvas* handle)
        : QGraphicsItem(nullptr)
        , mpHandle(handle)
    {
        setAcceptHoverEvents(true);
    }

public:
    BKCanvas* mpHandle = nullptr;
    QImage mCanvasImage;
    BackgroundSetting mBackgroundFillRule = BackgroundSetting::FillWhite;

public:
    QRectF boundingRect() const override {
        return { 0, 0, mpHandle->mSize.width(), mpHandle->mSize.height() };
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static_cast<void*>(widget);
        painter->drawImage(0, 0, mCanvasImage);
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        event->accept();
    }

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        event->accept();

        QPainter painter(&mCanvasImage);
        painter.drawLine(event->pos(), event->lastPos());
        painter.end();

        if (mpHandle->mCallbackFunc) {
            mpHandle->mCallbackFunc(mpHandle, QVariant::fromValue(&mCanvasImage));
        }

        update();
    }

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
        setCursor(Qt::PointingHandCursor);
    }

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
        setCursor(Qt::ArrowCursor);
    }
};

BKCanvas::BKCanvas()
    : super()
    , mpImpl(new Impl(this))
{
}

BKCanvas::~BKCanvas()
{
    delete mpImpl;
    mpImpl = nullptr;
}

BKCanvas* BKCanvas::setBackgroundType(BackgroundSetting setting)
{
    L_IMPL(BKCanvas);
    l->mBackgroundFillRule = setting;
    l->update();

    return this;
}

bool BKCanvas::loadFromJson(const QJsonValue& val)
{
    return true;
}

QVariant BKCanvas::data()
{
    return true;
}

BKCanvas::operator QJsonValue() const
{
    return true;
}

BKUnit* BKCanvas::copy()
{
    L_IMPL(BKCanvas);
    BKCanvas* target = BKCreator::create<BKCanvas>();

    BKCanvas::Impl* dstImpl = target->mpImpl;
    dstImpl->mCanvasImage = l->mCanvasImage;
    dstImpl->mBackgroundFillRule = l->mBackgroundFillRule;
    _copyBasicAttributeTo(target);

    return target;
}

QGraphicsItem* BKCanvas::getGraphicsItem()
{
    return mpImpl;
}

void BKCanvas::dataChanged(const QVariant& data)
{
    // Nothing
    return;
}

static_assert(static_cast<std::underlying_type_t<BKCanvas::BackgroundSetting>>(BKCanvas::BackgroundSetting::Count) == 2, "Only white and transparent are treated");
void BKCanvas::resized()
{
    L_IMPL(BKCanvas);

    // 不进行已有图片缩放，而是进行像素拷贝和裁剪
    QImage dst = QImage(mSize.width(), mSize.height(), QImage::Format::Format_RGBA8888);
    dst.fill(l->mBackgroundFillRule == BackgroundSetting::FillWhite ?  Qt::white : Qt::transparent);

    QPainter painter(&dst);
    painter.drawImage(0, 0, l->mCanvasImage);
    painter.end();

    l->mCanvasImage = dst;

    if (mCallbackFunc) {
        mCallbackFunc(this, QVariant::fromValue(&l->mCanvasImage));
    }
}
