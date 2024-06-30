#include "container/BKCard.h"
#include <QGraphicsItemGroup>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include "container/BKCell.h"
#include "BKCreator.hpp"
#include "BlueprintLoader.h"

class BKCard::Impl : public QGraphicsItemGroup
{
public:
    QString mstrTitle = "(empty)";

    BKCard* mpHandle = nullptr;

    std::vector<BKCell*> mItems;

    bool mbExpanded = true;
    // The size after becoming a group
    QSizeF mSize;

    static constexpr float mnVerticalSpacing = 5;
    static constexpr int32_t mnHeaderHeight = 30;

    QGraphicsDropShadowEffect* mpEffext = nullptr;
    // Whether it is being released
    bool mbImmortal = false;

public:
    Impl(BKCard* handle)
        :mpHandle(handle)
    {
        setHandlesChildEvents(false);
        this->setFlags(
            QGraphicsItem::ItemSendsScenePositionChanges
            | QGraphicsItem::ItemIsFocusable
            | QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable);

        mpEffext = new QGraphicsDropShadowEffect();
        mpEffext->setOffset(2, 2);
        mpEffext->setColor(Qt::black);
        mpEffext->setBlurRadius(5);
        this->setGraphicsEffect(mpEffext);
    }

    ~Impl()
    {
        mbImmortal = true;

        if (scene()) {
            scene()->removeItem(this);
        }

        for (auto& item : mItems)
        {
            removeFromGroup(reinterpret_cast<QGraphicsItem*>(item->mpImpl));
            delete item;
            item = nullptr;
        }
        mItems.clear();

        delete mpEffext;
        mpEffext = nullptr;
    }

    virtual QRectF boundingRect() const
    {
        if (mbExpanded)
            return { QPointF(0, 0), mSize };
        else
            return { 0, 0, mSize.width(), mnHeaderHeight };
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        // paint border
        painter->save();
        {
            //painter->setPen(theme);
            painter->setBrush(Qt::white);
            painter->drawRoundedRect(boundingRect(), 2, 2);
            painter->restore();
        }

        // paint title
        painter->save();
        {
            painter->setPen(Qt::black);
            painter->setBrush(Qt::NoBrush);
            static const float spacing = 6.0f;
            painter->drawText({ spacing, 0, 1.0f * mSize.width() - mnHeaderHeight - spacing, mnHeaderHeight }, mstrTitle, QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
            painter->restore();
        }
    }

    void reCalcSelf();

protected:
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override
    {
        /*
            If there are multiple associated cards moving at the same time, this can be optimized
            todo...
        */
        if (change == ItemPositionHasChanged) {
            for (auto& item : mItems) {
                item->dispatchPositionChanged();
            }
        }

        return QGraphicsItemGroup::itemChange(change, value);
    }
};

void BKCard::Impl::reCalcSelf()
{
    std::vector<qreal> heights;
    heights.reserve(mItems.size());

    qreal width = 0, height = mnHeaderHeight;
    for (auto& item : mItems)
    {
        auto size = item->getTheorySize();
        width = std::max(size.width(), width);
        heights.push_back(size.height());
    }

    int index = 0;
    for (int i = 0; i < mItems.size(); ++i)
    {
        BKCell* cell = mItems[i];

        QGraphicsItem* pItem =cell->getGraphicsItem();
        cell->updateActualSize({ width, heights[i] });
        cell->bindCard(mpHandle, index++);
        
        addToGroup(pItem);
        pItem->setPos({ 0, height });
        height += heights[i];
    }


    mSize = { width, height };
}

BKCard::BKCard()
    : StandAloneUnit(StandAloneUnit::Type::Card)
    , mpImpl(new Impl(this))
{

}

BKCard::~BKCard()
{
    delete mpImpl;
    mpImpl = nullptr;
}

void BKCard::setTitle(const QString& title)
{
    mpImpl->mstrTitle = title;
}

bool BKCard::loadFromJson(const QJsonArray& obj, const QPoint& pos)
{
    L_IMPL(BKCard);

    bool ok = true;
    for (int i = 0; i < l->mItems.size(); ++i)
    {
        auto cell = l->mItems[i];
        if (!cell->updateUnitFromJson(obj[i]))
        {
            ok = false;
            break;
        }
    }

    mpImpl->setPos(pos);
    return true;
}

QJsonArray BKCard::exportToJson()
{
    L_IMPL(BKCard)

    bool ok = true;
    QJsonArray rows;
    for (auto item : l->mItems)
    {
        if (!item->exportUnitToJson(rows))
        {
            ok = false;
            break;
        }
    }

    return ok ? rows : QJsonArray();
}

BKAnchor* BKCard::getRowAnchor(int row, BKAnchor::AnchorType type)
{
    L_IMPL(BKCard)

    if (row >= l->mItems.size())
        return nullptr;

    return l->mItems[row]->getAnchor(type);
}

bool BKCard::isStillAlive()
{
    L_IMPL(BKCard);
    return !(l->mbImmortal);
}

void BKCard::rePackage()
{
    L_IMPL(BKCard);

    for (BKCell* cell : l->mItems) 
    {
        QGraphicsItem* gi = cell->getGraphicsItem();
        l->removeFromGroup(gi);
        gi->setParentItem(nullptr);
    }
    l->reCalcSelf();

    for (BKCell* cell : l->mItems) {
        cell->dispatchPositionChanged();
    }
}

void BKCard::_pack(std::initializer_list<BKCell*> cells)
{
    L_IMPL(BKCard);

    for (auto& item : l->mItems) {
        l->removeFromGroup(reinterpret_cast<QGraphicsItem*>(item->getGraphicsItem()));
    }
    l->mItems.clear();

    // Add units, but do not addToGroup
    for (auto cell : cells) {
        l->mItems.push_back(cell);
    }

    l->reCalcSelf();
}

QGraphicsItem* BKCard::getBindItem()
{
    return mpImpl;
}

QVariant BKCard::getCurrentCardValue()
{
    return QVariant();
}
