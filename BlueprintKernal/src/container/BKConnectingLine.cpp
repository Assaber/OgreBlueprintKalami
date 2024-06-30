#include "container/BKConnectingLine.h"
#include "container/BKAnchor.h"
#include "container/BKCell.h"
#include "container/BKCard.h"
#include "unit/BKPushButton.h"

#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QStyleOption>
#include <QPainter>
#include <array>
#include <stdexcept>

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
        if (scene()) {
            scene()->removeItem(this);
        }

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

        // Be selected todo...
    }

    virtual QRectF boundingRect() const override 
    {
        return mBoundRect;
    }

    virtual bool sceneEvent(QEvent* event)
    {
        if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            auto mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
            QRectF range(mouseEvent->pos().x() - mnAnchorLineWidth,
                mouseEvent->pos().y() - mnAnchorLineWidth,
                mnAnchorLineWidth * 2,
                mnAnchorLineWidth * 2);

            if (!mPath.intersects(range)) {
                event->ignore();
            }
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

        BKAnchor* oa = mAnchorArray[0];
        BKAnchor* ia = mAnchorArray[1];
        
        if (regist) {
            oa->appendRegist(ia, mpHandle); ia->appendRegist(oa, mpHandle);
        }
        else {
            ia->removeRegist(oa); oa->removeRegist(ia);

            BKCard *oc = oa->getBindCard(), *ic = ia->getBindCard();
            if (ia->getAnchorType() & BKAnchor::AnchorType::MultiConn)
            {
                if (oa->getRegistUnits().size() == 0                        // The output anchor has no registed unit, directly bound to the card
                    && ic->isStillAlive()) {                                // The input card has not been released
                    ia->dataChanged(oc->getCurrentCardValue());
                }
            }
            else if(ic->isStillAlive()) {                                   // The input card has not been released
                ia->dataChanged();
            }
        }

        oa->update(); ia->update();
    }

    void standardAnchors() {
        // Either none at all, or update the layout
        if ((mAnchorArray[0] && !mAnchorArray[1])
            || (!mAnchorArray[0] && mAnchorArray[1]))
        {
            throw std::logic_error("Unmatched anchors' layout!!!! why??????");
        }

        if (!mAnchorArray[0] && !mAnchorArray[1])
            return;
        else if (mAnchorArray[0]->getAnchorType() & BKAnchor::Input)
            std::swap(mAnchorArray[0], mAnchorArray[1]);
    }

public:
    BKConnectingLine* mpHandle = nullptr;

    static constexpr float mnAnchorLineWidth = 4.0f;

    std::array<BKAnchor*, 2> mAnchorArray = { nullptr, nullptr };

    QPen mPen;
    QPainterPath mPath;
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
    L_IMPL(BKConnectingLine);

    l->setPen(QPen(color, l->mnAnchorLineWidth));
    return *this;
}

BKConnectingLine& BKConnectingLine::setVisible(bool visible)
{
    L_IMPL(BKConnectingLine);

    l->setVisible(visible);
    return *this;
}

BKConnectingLine& BKConnectingLine::setPainterPath(const QPainterPath& path)
{
    L_IMPL(BKConnectingLine);

    l->setPath(path);
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
    L_IMPL(BKConnectingLine);

    l->update();
}

void BKConnectingLine::updateByBind()
{
    L_IMPL(BKConnectingLine);

    if (l->isFullyBinding())
    {
        QGraphicsItem* a1 = l->mAnchorArray[0]->getGraphicsItem();
        QGraphicsItem* a2 = l->mAnchorArray[1]->getGraphicsItem();

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

    auto units = anchor->getRegistUnits();
    if (units.size()) 
    {
        for (auto unit : units) {
            unit->dataChanged(QVariant());
        }
    }
    else if (anchor->mpBindCard) {
        anchor->dataChanged(anchor->mpBindCard->getCurrentCardValue());
    }
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
