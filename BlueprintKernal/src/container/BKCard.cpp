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
    // 标题内容
    QString mstrTitle = "(empty)";
    // 关联句柄
    BKCard* mpHandle = nullptr;
    // 包含对象
    std::vector<BKCell*> mItems;
    // 是否处于展开状态
    bool mbExpanded = true;
    // 成组后自身大小
    QSizeF mSize;
    // 垂直方向间隔
    static constexpr float mnVerticalSpacing = 5;
    // 内部标头高度
    static constexpr int32_t mnHeaderHeight = 30;
    // 展开按钮中心圆圈半径
    static constexpr float mnRadius = 8.0f;
    // 阴影效果
    QGraphicsDropShadowEffect* mpEffext = nullptr;
    // 是否正在释放
    // 卡片释放时会导致锚点释放，锚点关联连接线可能又回来了（dataChanged），所以用一个标识更新释放状态
    // 理论上只有释放的时候会用得到
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
            | QGraphicsItem::ItemIsSelectable);        //追加的两个分别用于移动时反馈和按键时反馈

        mpEffext = new QGraphicsDropShadowEffect();
        mpEffext->setOffset(2, 2);
        mpEffext->setColor(Qt::black);
        mpEffext->setBlurRadius(5);
        this->setGraphicsEffect(mpEffext);
    }

    ~Impl()
    {
        mbImmortal = true;

        qDebug() << "card item delete";

        // 从场景中移除后分元释放
        if (scene())
            scene()->removeItem(this);

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
        // 绘制边框
        painter->save();
        {
            //painter->setPen(theme);
            painter->setBrush(Qt::white);
            painter->drawRoundedRect(boundingRect(), 2, 2);
            painter->restore();
        }

        // 绘制标题
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
        这里存在优化： 
            判断当前选择的卡片是否是多个
                - 如果是多个则判断二者中间是否存在关联
                    - 如果还存在关联，则只需要更新一次锚点位置变化

        assaber todo...
        */
        if (change == ItemPositionHasChanged)       
            for (auto& item : mItems)
                item->dispatchPositionChanged();
            

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

    for (BKCell* cell : l->mItems) {
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
    L_IMPL(BKCard)

    // 清除
    for (auto& item : l->mItems)
        l->removeFromGroup(reinterpret_cast<QGraphicsItem*>(item->getGraphicsItem()));
    l->mItems.clear();

    // 添加新单元，但是不进入到组
    for(auto cell : cells)
        l->mItems.push_back(cell);

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
