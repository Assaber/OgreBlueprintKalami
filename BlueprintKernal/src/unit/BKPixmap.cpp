#include "unit/BKPixmap.h"
#include "container/BKAnchor.h"
#include <QPainter>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>

class BKPixmap::Impl : public QGraphicsItem
{
public:
    Impl(BKPixmap* handle) : mpHandle(handle) {}

public:
    QRectF boundingRect() const override
    {
        return { 0, 0, mpHandle->mSize.width(), mpHandle->mSize.height() };
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static_cast<void*>(widget);
        painter->save();
        {
            painter->setPen(Qt::black);
            painter->drawText(mBoundingRect, "双击选择文件", QTextOption(Qt::AlignCenter));
            painter->restore();
        }

        painter->save();
        {
            painter->drawPixmap(mBoundingRect.topLeft(), mPixmap);
            painter->restore();
        }
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        event->accept();
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override
    {
        QString path = QFileDialog::getOpenFileName(nullptr, "", "./", "Images (*.png *.xpm *.jpg);;All files (*.*)");
        if (!path.isEmpty())
            mpHandle->dataChanged(path);
          
        event->accept();
    }

public:
    BKPixmap* mpHandle = nullptr;
    QString mstrSource = "";
    // 包围盒
    QRect mBoundingRect = { 0, 0, 60, 60 };
    // 图片
    QPixmap mPixmap;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
};

BKPixmap::BKPixmap()
    : super()
    , mpImpl(new Impl(this))
{
    setMinSize(mpImpl->mBoundingRect.size());
}

BKPixmap::~BKPixmap()
{
    delete mpImpl;
    mpImpl = nullptr;
}

QJsonValue BKPixmap::getValue()
{
    L_IMPL(BKPixmap);
    return l->mstrSource;
}

bool BKPixmap::setValue(const QJsonValue& val)
{
    setSource(val.toString());
    return true;
}

BKPixmap* BKPixmap::setSource(const QString& path)
{
    L_IMPL(BKPixmap);

    l->mstrSource = path;
    if(l->mBoundingRect.width())
        l->mPixmap = QPixmap(l->mstrSource).scaled(l->mBoundingRect.size());
    l->update();
    return this;
}

QGraphicsItem* BKPixmap::getGraphicsItem()
{
    return mpImpl;
}

void BKPixmap::dataChanged(const QVariant& data)
{ 
    L_IMPL(BKPixmap);

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->mstrSource);
    }
    else
    {
        setSource(data.toString());
        l->update();
        if (!mCallbackFunc(data) && mpRightAnchor)
            mpRightAnchor->dataChanged(data);
    }
}

void BKPixmap::resized()
{
    L_IMPL(BKPixmap);

    QSize size = mSize.toSize();
    size.setHeight(size.height() - 2 * l->mFixedMargin);
    int r = std::min(size.width(), size.height());
    if(r == size.width())
        l->mBoundingRect = { 0, (size.height() - r) / 2 + l->mFixedMargin, r, r };
    else
        l->mBoundingRect = { (size.width() - r) / 2, l->mFixedMargin, r, r };

    if(!l->mstrSource.isEmpty() && r > 0)
        l->mPixmap = QPixmap(l->mstrSource).scaled(l->mBoundingRect.size());
}
