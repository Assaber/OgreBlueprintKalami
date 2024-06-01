#include "unit/BKCheckBox.h"
#include "container/BKCard.h"
#include "BKEvent.h"

#include <QGraphicsSceneMouseEvent>
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyle>


class BKCheckBox::Impl : public QGraphicsItem
{
public:
    Impl(BKCheckBox* handle)
        : QGraphicsItem()
        , mpHandle(handle)
    {
    }

    ~Impl()
    {

    }

public:
    virtual QRectF boundingRect() const override
    {
        return mBoundingRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        // 绘制外边框
        QPen p(QColor(255, 128, 64));
        qreal r = mCtrlArea.height() / 2;
        painter->save();
        {
            painter->setPen(p);
            painter->setBrush(mbEnable ? QColor(255, 128, 64) : Qt::white);
            painter->drawRoundedRect(mCtrlArea, r, r);
            painter->restore();
        }

        // 绘制按钮
        painter->save();
        {
            painter->setPen(p);
            painter->setBrush(Qt::white);
            if(mbEnable)
                painter->drawEllipse(QPointF(mCtrlArea.right() - r, r + mFixedMargin), r - 2.0f, r - 2.0f);
            else
                painter->drawEllipse(QPointF(r ,r + mFixedMargin), r - 3.0f, r - 3.0f);
            painter->restore();
        }
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (!mpHandle->mbEnable)
        {
            event->ignore();
            return;
        }

        mpHandle->dataChanged(!mbEnable);
        update();
    }

public:
    BKCheckBox* mpHandle = nullptr;
    // 包围盒
    QRectF mBoundingRect;
    // 控件区域
    QRect mCtrlArea;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 使能
    bool mbEnable = false;
};


BKCheckBox::BKCheckBox()
    : super()
    , mpImpl(new Impl(this))
{
    setFixedSize({ fixedWidth, 20 });
}

BKCheckBox::~BKCheckBox()
{
    delete mpImpl;
    mpImpl = nullptr;
}


bool BKCheckBox::loadFromJson(const QJsonValue& val)
{
    L_IMPL(BKCheckBox);
    setChecked(val.toBool());

    return true;
}

BKCheckBox::operator QJsonValue() const
{
    L_IMPL(BKCheckBox);
    return { l->mbEnable };
}

QVariant BKCheckBox::data()
{
    L_IMPL(BKCheckBox);
    return { l->mbEnable };
}

BKUnit* BKCheckBox::copy()
{
    L_IMPL(BKCheckBox);
    BKCheckBox* target = BKCreator::create<BKCheckBox>();
    _copyBasicAttributeTo(target);

    return target;
}

BKCheckBox* BKCheckBox::setChecked(bool enable)
{
    L_IMPL(BKCheckBox)

    l->mbEnable = enable;
    return this;
}

QGraphicsItem* BKCheckBox::getGraphicsItem()
{
    return mpImpl;
}

void BKCheckBox::resized()
{
    L_IMPL(BKCheckBox)

    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mCtrlArea = { 0, l->mFixedMargin, static_cast<int>(l->mBoundingRect.width()), static_cast<int>(l->mBoundingRect.height() - 2 * l->mFixedMargin) };
}

void BKCheckBox::dataChanged(const QVariant& data)
{
    L_IMPL(BKCheckBox);

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->mbEnable);
    }
    else
    {
        setChecked(data.toBool());
        l->update();
        if (!mCallbackFunc(data) && mpRightAnchor)
            mpRightAnchor->dataChanged(data);
    }
}
