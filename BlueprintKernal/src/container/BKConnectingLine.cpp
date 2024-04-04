﻿#include "container/BKConnectingLine.h"
#include "container/BKAnchor.h"
#include <QGraphicsScene>
#include <array>
#include <QPainter>
#include <QStyleOption>
#include "container/BKCell.h"
#include "unit/BKPushButton.h"
#include <QGraphicsSceneEvent>

class BKConnectingLine::Impl : public QGraphicsItem
{
public:
    Impl(BKConnectingLine* handle, BKAnchor* a1, BKAnchor* a2)
        : QGraphicsItem(nullptr)
        , mpHandle(handle)
        , mAnchorArray({ a1, a2 })
    {
        standardAnchors();

        setZValue(-1);
        setFlag(QGraphicsItem::ItemIsSelectable);

        anchorControl(true);
    }

    ~Impl()
    {
        if (scene())
            scene()->removeItem(this);

        anchorControl(false);
    }

public:
    void setPen(const QPen& pen) { mPen = pen; }
    void setPath(const QPainterPath& path) { 
        mPath = path; 
        mBoundRect = mPath.boundingRect();
    }

public:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        Q_UNUSED(widget);
        painter->setPen(mPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(mPath);

        // 选中 todo...
    }

    virtual QRectF boundingRect() const override {
        return mBoundRect;
    }

    virtual bool sceneEvent(QEvent* event)
    {
        if (event->type() == QEvent::GraphicsSceneMousePress)           // 如果按照包围盒算，可能会选中错误
        {
            auto mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
            QRectF range(mouseEvent->pos().x() - mnAnchorLineWidth,
                mouseEvent->pos().y() - mnAnchorLineWidth,
                mnAnchorLineWidth * 2,
                mnAnchorLineWidth * 2);

            if (!mPath.intersects(range))
                event->ignore();
        }

        return QGraphicsItem::sceneEvent(event);
    }

public:
    inline bool isFullyBinding() 
    {
        return mAnchorArray[0] && mAnchorArray[1];
    }

    void anchorControl(bool regist)
    {
        if (!isFullyBinding())
            return;
        
        if (regist)
        {
            mAnchorArray[0]->appendRegist(mAnchorArray[1], mpHandle);
            mAnchorArray[1]->appendRegist(mAnchorArray[0], mpHandle);
        }
        else
        {
            mAnchorArray[0]->removeRegist(mAnchorArray[1]);
            mAnchorArray[1]->removeRegist(mAnchorArray[0]);
        }

        mAnchorArray[0]->update();
        mAnchorArray[1]->update();
    }

    void standardAnchors() {
        // 要么就都没有，要么更新布局
        if ((mAnchorArray[0] && !mAnchorArray[1])
            || (!mAnchorArray[0] && mAnchorArray[1]))
        {
            throw std::exception("查！严查！！为什么会有不配套的传入");
        }

        if (!mAnchorArray[0] && !mAnchorArray[1])
            return;
        else if (mAnchorArray[0]->getAnchorType() == BKAnchor::Input)
            std::swap(mAnchorArray[0], mAnchorArray[1]);
    }

public:
    BKConnectingLine* mpHandle = nullptr;
    // 锚线宽度
    static constexpr float mnAnchorLineWidth = 4.0f;
    // 绑定的锚点
    std::array<BKAnchor*, 2> mAnchorArray = { nullptr, nullptr };
    // 画笔
    QPen mPen;
    // 路径
    QPainterPath mPath;
    // 包围盒
    QRectF mBoundRect;
};


BKConnectingLine::BKConnectingLine(const QColor& color, BKAnchor* a1, BKAnchor* a2)
    : StandAloneUnit(StandAloneUnit::Type::ConnectingLine)
    , mpImpl(new Impl(this, a1, a2))
{
    setColor(color);

    updateByBind();
    triggerOriginOutput();
}

BKConnectingLine::BKConnectingLine()
    : StandAloneUnit(StandAloneUnit::Type::ConnectingLine)
    , mpImpl(new Impl(this, nullptr, nullptr))
{

}

BKConnectingLine::~BKConnectingLine()
{
    delete mpImpl;
    mpImpl = nullptr;
}

BKConnectingLine& BKConnectingLine::setColor(const QColor& color)
{
    mpImpl->setPen(QPen(color, mpImpl->mnAnchorLineWidth));
    return *this;
}

BKConnectingLine& BKConnectingLine::setVisible(bool visible)
{
    mpImpl->setVisible(visible);
    return *this;
}

BKConnectingLine& BKConnectingLine::setPainterPath(const QPainterPath& path)
{
    mpImpl->setPath(path);
    return *this;
}

BKConnectingLine& BKConnectingLine::setPainterPath(const QPointF& begin)
{
    return setPainterPath(createPainterPath(&begin));
}

BKConnectingLine& BKConnectingLine::setPainterPath(const QPointF& begin, const QPointF& end)
{
    return setPainterPath(createPainterPath(&begin, &end));
}

void BKConnectingLine::update()
{
    mpImpl->update();
}

void BKConnectingLine::updateByBind()
{
    if (mpImpl->isFullyBinding())
    {
        QGraphicsItem* a1 = mpImpl->mAnchorArray[0]->getGraphicsItem();
        QGraphicsItem* a2 = mpImpl->mAnchorArray[1]->getGraphicsItem();

        QPointF p1 = a1->mapToScene(a1->boundingRect().center());
        QPointF p2 = a2->mapToScene(a2->boundingRect().center());

        setPainterPath(p1, p2);
    }

    update();
}

void BKConnectingLine::triggerOriginOutput()
{
    L_IMPL(BKConnectingLine);

    auto anchor = l->mAnchorArray[0];
    if (!anchor)
        return;

    for (auto unit : anchor->getRegistUnits())
        unit->dataChanged(QVariant());
}

QPainterPath BKConnectingLine::createPainterPath(const QPointF* begin, const QPointF* end /*= nullptr*/)
{
    QPainterPath ret(*begin);
    if (end)
    {
        qreal mid = (begin->x() + end->x()) / 2;
        ret.cubicTo({ mid, begin->y() }, { mid, end->y() }, *end);
    }

    return ret;
}


bool BKConnectingLine::getBasicInfo(BasicInfo& info)
{
    L_IMPL(BKConnectingLine);

    if (!l->mAnchorArray[0] || !l->mAnchorArray[1])
        return false;

    auto rAnchor = l->mAnchorArray[0];
    info.rCard = rAnchor->getBindCard();
    info.rIndex = rAnchor->getCell()->getMemberRow(rAnchor);

    auto lAnchor = l->mAnchorArray[1];
    info.lCard = lAnchor->getBindCard();
    info.lIndex = lAnchor->getCell()->getMemberRow(lAnchor);

    return true;
}

QGraphicsItem* BKConnectingLine::getBindItem()
{
    return mpImpl;
}
