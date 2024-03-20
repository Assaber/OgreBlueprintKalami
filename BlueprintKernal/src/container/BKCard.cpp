#include "container/BKCard.h"
#include <QGraphicsItemGroup>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

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

void BKCard::_pack(std::initializer_list<BKCell*> cells)
{
    // 清除
    for (auto& item : mpImpl->mItems)
        mpImpl->removeFromGroup(reinterpret_cast<QGraphicsItem*>(item->mpImpl));
    mpImpl->mItems.clear();

    std::vector<qreal> heights;
    heights.reserve(cells.size());

    qreal width = 0, height = mpImpl->mnHeaderHeight;
    for (auto& item : cells)
    {
        mpImpl->mItems.push_back(item);

        auto size = item->getTheorySize();
        width = std::max(size.width(), width);
        heights.push_back(size.height());
    }

    for (int i = 0; i < mpImpl->mItems.size(); ++i)
    {
        BKCell* cell = mpImpl->mItems[i];
        cell->updateActualSize({ width, heights[i] });
        cell->bindCard(this);

        QGraphicsItem* pItem = reinterpret_cast<QGraphicsItem*>(cell->mpImpl);
        pItem->setY(height);
        mpImpl->addToGroup(pItem);
        height += heights[i];
    }

    mpImpl->mSize = { width, height };
}

QGraphicsItem* BKCard::getBindItem()
{
    return mpImpl;
}
