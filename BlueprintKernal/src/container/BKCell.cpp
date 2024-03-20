#include "container/BKCell.h"
#include <array>
#include <QGraphicsItemGroup>
#include <QDebug>

class BKCell::Impl : public QGraphicsItemGroup
{
public:
    Impl(BKCell* handle, BKAnchor::AnchorType type)
        : QGraphicsItemGroup()
        , mpHandle(handle)
    {
        // 支持让子组件可以处理自己的事件，比如combobox
        setHandlesChildEvents(false);

        if (type & BKAnchor::AnchorType::Input)
            mAnchorArray[0] = new BKAnchor(BKAnchor::AnchorType::Input, mpHandle);

        if(type & BKAnchor::AnchorType::Output)
            mAnchorArray[1] = new BKAnchor(BKAnchor::AnchorType::Output, mpHandle);
    }

    ~Impl()
    {
        qDebug() << "cell item destroy";
        // 不通过Qt的GraphicsItem释放机制，解组后分批释放
        removeAll(true);
        destroyAnchorBall();
    }

public:
    /**
     * @brief:                从组中移除对象
     * @return: void
     * @remark: 
     */
    void removeAll(bool destroy = false) {
        for (auto& item : mUnits)
        {
            removeFromGroup(item->getGraphicsItem());
            if (destroy)
                delete item;
        }

        if (destroy)
            mUnits.clear();
    }

    void destroyAnchorBall() {
        for (auto& anchor : mAnchorArray)
        {
            if (!anchor)
                continue;

            removeFromGroup(anchor->getGraphicsItem());

            delete anchor;
            anchor = nullptr;
        }
    }

public:
    QSizeF getTheorySize();
    void updateActualSize(const QSizeF& aim);
    void dispatchPositionChanged();

public:
    BKCell* mpHandle = nullptr;
    // 单元列表
    std::vector<BKUnit*> mUnits;
    // 最小高度
    static constexpr int mnMinHeight = 20;
    // 组元最小间隔
    static constexpr int mnSpacing = 5;
    // 锚点
    std::array<BKAnchor*, 2> mAnchorArray = { nullptr, nullptr };
    // 锚点在自身控件外的间隔
    static constexpr float mnAnchorBallPadding = 6;
    // 锚点固定宽度（宽高可能不同相同）
    static constexpr float mnAnchorFixedWidth = (mnAnchorBallPadding + BKAnchor::mnAnchorBallRadius) * 2;
};

BKCell::BKCell(BKAnchor::AnchorType type)
    : mpImpl(new Impl(this, type))
{

}

BKCell::~BKCell()
{
    delete mpImpl;
    mpImpl = nullptr;
}

BKCell* BKCell::append(std::initializer_list<BKUnit*> units)
{
    for (auto item : units)
        mpImpl->mUnits.push_back(item);

    return this;
}

BKCell* BKCell::append(BKUnit* unit)
{
    mpImpl->mUnits.push_back(unit);
    return this;
}

void BKCell::updateActualSize(const QSizeF& aim) { mpImpl->updateActualSize(aim); }
void BKCell::Impl::updateActualSize(const QSizeF& aim)
{
    removeAll();

    /* 对于自适应的宽度
    *    组元宽度 = (总宽度 - 全部固定宽度 - 总间隔 - 左右锚点占位) / 自适应组元个数;
    */

    float fixed = 0;
    float adaptive = 0;
    std::vector<BKUnit*> aus;
    auto update_unit_y = [&](BKUnit* unit) {
        if (unit->mSizePolicy == BKUnit::SizePolicy::Fixed)
            fixed += unit->mMinSize.width();
        else
            aus.push_back(unit);

        QGraphicsItem* pItem = unit->getGraphicsItem();
        if (unit->mSizePolicy == BKUnit::SizePolicy::Adaptive)        //非固定组元撑起高度
        {
            unit->mSize.setHeight(aim.height());
            pItem->setY(0);
        }
        else                                                        //自适应的组元拷贝高度
        {
            unit->mSize.setHeight(unit->mMinSize.height());
            pItem->setY((aim.height() - unit->mMinSize.height()) / 2);
        }
    };


    if (mAnchorArray[0])
    {
        fixed += 2 * mnAnchorBallPadding;
        update_unit_y(mAnchorArray[0]);
    }
    else
        fixed += mnAnchorFixedWidth;
        

    for (auto& unit : mUnits)
        update_unit_y(unit);

    if (mAnchorArray[1])
    {
        fixed += 2 * mnAnchorBallPadding;
        update_unit_y(mAnchorArray[1]);
    }
    else
        fixed += mnAnchorFixedWidth;

    if (aus.size())
    {
        adaptive = aim.width() - fixed - (mUnits.size() > 1 ? mnSpacing * (mUnits.size() - 1) : 0);
        adaptive /= aus.size();
    }


    auto update_unit_x = [this, adaptive](BKUnit* unit, int offset, int& begin) {
        begin += offset;

        if (unit->mSizePolicy == BKUnit::SizePolicy::Fixed)
            unit->mSize.setWidth(unit->mMinSize.width());
        else
            unit->mSize.setWidth(adaptive);

        unit->getGraphicsItem()->setX(begin);
        begin += unit->mSize.width();

        //加入到组
        addToGroup(unit->getGraphicsItem());
    };

    int begin = 0;
    if (mAnchorArray[0])
    {
        begin += mnAnchorBallPadding;
        update_unit_x(mAnchorArray[0], 0, begin);
        begin += mnAnchorBallPadding;
    }
    else
        begin += mnAnchorFixedWidth;

    begin -= mnSpacing;
    for (auto& item : mUnits)
    {
        update_unit_x(item, mnSpacing, begin);
        item->resized();
    }
        

    if (mAnchorArray[1])
    {
        begin += mnAnchorBallPadding;
        update_unit_x(mAnchorArray[1], 0, begin);
        begin += mnAnchorBallPadding;
    }
    else
        begin += mnAnchorFixedWidth;
}


QSizeF BKCell::getTheorySize() { return mpImpl->getTheorySize(); }

void BKCell::setAnchorOffset(qreal offset)
{
    for (auto anchor : mpImpl->mAnchorArray)
    {
        if (anchor)
            anchor->setBezierOffset(static_cast<int>(offset));
    }
}

bool BKCell::hasAnchor(BKAnchor* anchor)
{
    L_IMPL(BKCell)
    return  l->mAnchorArray[0] == anchor || l->mAnchorArray[1] == anchor;
}

void BKCell::dispatchPositionChanged() { return mpImpl->dispatchPositionChanged(); }

void BKCell::bindCard(BKCard* card)
{
    L_IMPL(BKCell)
    for (auto anchor : l->mAnchorArray)
    {
        if (anchor)
            anchor->mpBindCard = card;
    }

    for (auto unit : l->mUnits)
        unit->mpBindCard = card;
}

void BKCell::Impl::dispatchPositionChanged()
{
    for (const auto& anchor : mAnchorArray)
    {
        if (anchor)
            anchor->dispatchCellPositionChanged();
    }
}

QSizeF BKCell::Impl::getTheorySize()
{
    float width = 0, height = mnMinHeight;

    // 根据锚点设置更新最小高度
    bool hasAnchor = mAnchorArray[0] || mAnchorArray[1];
    if (hasAnchor)
        height = std::max(height, BKAnchor::mnAnchorBallRadius * 2);

    for (auto& item : mUnits)
    {
        if (height < item->mMinSize.height())
            height = item->mMinSize.height();

        width += item->mMinSize.width();
    }

    int count = mUnits.size();
    if (count > 1)
        width += mnSpacing * (count - 1);

    //左右锚点占位
    width += 2 * mnAnchorFixedWidth;

    return { width, height };
}