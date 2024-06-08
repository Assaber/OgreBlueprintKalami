#include "container/BKCell.h"
#include <array>
#include <QGraphicsItemGroup>
#include <stdexcept>
#include <QDebug>
#include <QPainter>
#include "BKCreator.hpp"
#include "unit/BKSpacer.h"
#include "unit/BKPushButton.h"
#include "container/BKCard.h"

class BKCell::Impl : public QGraphicsItemGroup
{
public:
    Impl(BKCell* handle, uint32_t l, uint32_t r, BKCell::Type type)
        : QGraphicsItemGroup()
        , mpHandle(handle)
        , mType(type)
    {
        // 支持让子组件可以处理自己的事件，比如combobox
        setHandlesChildEvents(false);

        if (l != BKAnchor::None)
            mAnchorArray[0] = new BKAnchor(l &(~BKAnchor::AnchorType::Output), mpHandle);

        if(r != BKAnchor::None)         //输出锚点自带多重绑定
            mAnchorArray[1] = new BKAnchor((r & (~BKAnchor::AnchorType::Input)) | BKAnchor::MultiConn, mpHandle);

        if (type == Type::ListGroup)
        {
            mUnitOffset = 3;            // 尽量不要改此处的3， 他影响到宽度的计算

            mUnits.push_back(BKCreator::create<BKSpacer>());
            BKPushButton* pushBtn = BKCreator::create<BKPushButton>();
            pushBtn->setText("+");
            pushBtn->setFixedSize({ mnGroupBtnWidth, mnGroupBtnWidth });
            pushBtn->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) ->bool {
                if (mpHandle->push() && mMenberCountChangedFunc) {
                    QVariantList ret;
                    for (int i = mUnitOffset; i < mUnits.size(); ++i)
                    {
                        BKUnit* unit = mUnits[i];
                        ret.push_back(unit->data());
                    }
                    mMenberCountChangedFunc(getGroupRows(), ret);
                }
                return true;
                });

            mUnits.push_back(pushBtn);

            BKPushButton* popBtn = BKCreator::create<BKPushButton>();
            popBtn->setText("-");
            popBtn->setFixedSize({ mnGroupBtnWidth, mnGroupBtnWidth });
            popBtn->setDataChangeCallback([this](BKUnit* unit, const QVariant& data) ->bool {
                if (mpHandle->pop() && mMenberCountChangedFunc) {
                    QVariantList ret;
                    for (int i = mUnitOffset; i < mUnits.size(); ++i)
                    {
                        BKUnit* unit = mUnits[i];
                        ret.push_back(unit->data());
                    }
                    mMenberCountChangedFunc(getGroupRows(), ret);
                }
                return true;
                });

            mUnits.push_back(popBtn);
        }
    }

    ~Impl()
    {
        qDebug() << "cell item destroy";
        // 不通过Qt的GraphicsItem释放机制，解组后分批释放

        destroyAnchorBall();
        removeAll();
        destroyUnits();
    }

public:
    /**
     * @brief:                从组中移除对象
     * @return: void
     * @remark: 
     */
    void removeAll() {
        for (auto& item : mUnits)
        {
            removeFromGroup(item->getGraphicsItem());
        }
    }

    void destroyUnits() {
        for (auto& item : mUnits)
            delete item;
        
        for (auto& item : mTemplateUnits)
            delete item;

        mUnits.clear();
        mTemplateUnits.clear();
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

    size_t getGroupRows() const 
    {
        size_t templateUnitCount = mTemplateUnits.size();
        size_t realUnitCount = mUnits.size() - mUnitOffset;

        return (templateUnitCount == 0) ? 0 : realUnitCount / templateUnitCount;
    }
public:
    BKCell* mpHandle = nullptr;
    // 单元列表
    std::vector<BKUnit*> mUnits;
    // 列表偏移：
    // 对于Group来讲，会内置三个控件：水平弹簧 +按钮 -按钮
    int mUnitOffset = 0;
    // 模板列表
    std::vector<BKUnit*> mTemplateUnits;
    // 通过模板创建的最大条数
    int mnMembersCountLimit = std::numeric_limits<int>::max();
    // 组元最小间隔
    static constexpr int mnSpacing = 5;
    // 锚点
    std::array<BKAnchor*, 2> mAnchorArray = { nullptr, nullptr };
    // 锚点在自身控件外的间隔
    static constexpr float mnAnchorBallPadding = 6;
    // 锚点固定宽度（宽高可能不同相同）
    static constexpr float mnAnchorFixedWidth = (mnAnchorBallPadding + BKAnchor::mnAnchorBallRadius) * 2;
    // 在卡片的行数
    int mRow = -1;
    // 类型
    BKCell::Type mType;
    // 成组按钮大小
    static constexpr int mnGroupBtnWidth = 30;
    // 绑定的卡片
    BKCard* mpBindCard = nullptr;
    // 成组单元个数改变的回调
    GroupMemberChangedFunc mMenberCountChangedFunc = nullptr;
    GroupMemberChangedFunc mMenberDataChangedFunc = nullptr;
};

BKCell::BKCell(BKAnchor::AnchorType anchor, Type type)
    : mpImpl(new Impl(this, 
        static_cast<BKAnchor::AnchorType>(anchor& BKAnchor::AnchorType::Input),
        static_cast<BKAnchor::AnchorType>(anchor& BKAnchor::AnchorType::Output),
        type))
{

}

BKCell::BKCell(uint32_t l, uint32_t r, Type type)
    : mpImpl(new Impl(this, l, r, type))
{

}

BKCell::~BKCell()
{
    delete mpImpl;
    mpImpl = nullptr;
}

BKCell* BKCell::append(std::initializer_list<RegistableUnitPair> units)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::SingleLine && "This method can only be used in single-line mode");

    for (auto item : units)
        append(item.unit, item.regist);
        
    return this;
}

BKCell* BKCell::append(BKUnit* unit, bool regist/* = true*/)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::SingleLine && "This method can only be used in single-line mode");

    if (regist && l->mAnchorArray[0])
        l->mAnchorArray[0]->appendRegist(unit);

    // 保险起见还是遍历一遍看是否存在重复传入的
    // 因为自己就传错了
    bool hasRepead = false;
    for (BKUnit* ui : l->mUnits)
    {
        if (ui == unit)
        {
            hasRepead = true;
            break;
        }
    }
    assert(!hasRepead && "There are duplicates in the incoming object");


    l->mUnits.push_back(unit);

    if (regist && l->mAnchorArray[1])
    {
        unit->mpRightAnchor = l->mAnchorArray[1];
        l->mAnchorArray[1]->appendRegist(unit);
    }
        
    return this;
}

BKCell* BKCell::setTemplate(std::initializer_list<BKUnit*> units)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::ListGroup && "This method can only be used in list-group mode");

    if (units.size() == 0)
        throw std::logic_error("Aha ? Why is an empty list passed in");

    l->mTemplateUnits.clear(); 
    for (BKUnit* unit : units)
        l->mTemplateUnits.push_back(unit);

    return this;
}

BKCell* BKCell::setMemberMaximum(int count)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::ListGroup && "This method can only be used in list-group mode");

    l->mnMembersCountLimit = count;
    return this;
}

BKCell* BKCell::setMemberDataChangedCallback(GroupMemberChangedFunc function)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::ListGroup && "This method can only be used in list-group mode");

    l->mMenberDataChangedFunc = function;
    return this;
}

BKCell* BKCell::setMemberCountChangedCallback(GroupMemberChangedFunc function)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::ListGroup && "This method can only be used in list-group mode");

    l->mMenberCountChangedFunc = function;
    return this;
}

bool BKCell::push(uint32_t count/* = 1*/)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::ListGroup && "This method can only be used in list-group mode");

    while (count-- && (l->getGroupRows() < l->mnMembersCountLimit))
    {
        for (auto unit : l->mTemplateUnits)
        {
            BKUnit* copyItem = unit->copy();
            QGraphicsItem* gi = copyItem->getGraphicsItem();
            l->addToGroup(gi);

            copyItem->mCallbackFunc = [this, l](BKUnit* unit, const QVariant& param) ->bool {
                if (l->mMenberDataChangedFunc)
                {
                    QVariantList ret;
                    for (int i = l->mUnitOffset; i < l->mUnits.size(); ++i)
                    {
                        BKUnit* unit = l->mUnits[i];
                        ret.push_back(unit->data());
                    }

                    l->mMenberDataChangedFunc(l->getGroupRows(), ret);
                }

                return true;        // 不给传递 TBD
            };

            l->mUnits.push_back(copyItem);
        }
    }

    // 触发卡片重新计算
    if (l->mpBindCard)
        l->mpBindCard->rePackage();

    return true;
}

bool BKCell::pop()
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::ListGroup && "This method can only be used in list-group mode");

    int count = l->mTemplateUnits.size();
    int size = l->mUnits.size();

    if (size == l->mUnitOffset)
        return false;

    auto itor = l->mUnits.begin();
    itor = std::next(itor, size - count);
    while (itor != l->mUnits.end())
    {
        BKUnit* unit = *itor;
        itor = l->mUnits.erase(itor);

        l->removeFromGroup(unit->getGraphicsItem());
        delete unit;
        unit = nullptr;
    }

    // 触发卡片重新计算
    if (l->mpBindCard)
        l->mpBindCard->rePackage();

    return true;
}

BKCell* BKCell::setDataType(BKAnchor::AnchorType anchor, uint32_t data)
{
    L_IMPL(BKCell)
    BKAnchor* la = l->mAnchorArray[0];
    BKAnchor* ra = l->mAnchorArray[1];

    if((anchor & BKAnchor::AnchorType::Input) && la)
        la->setDateType(data);

    if ((anchor & BKAnchor::AnchorType::Output) && ra)
        ra->setDateType(data);

    return this;
}

void BKCell::updateActualSize(const QSizeF& aim) 
{ 
    L_IMPL(BKCell);
    if(l->mType == BKCell::Type::ListGroup && l->mTemplateUnits.size() == 0)
        throw std::logic_error("已经抛一次异常了，为什么还会到这儿？！");

    if (l->mType == BKCell::Type::SingleLine && l->mUnits.size() == 0)   //一行中至少拥有一个控件，如果为空就用BKSpacer顶
        append(BKCreator::create<BKSpacer>());         

    // 先把锚点抛掉，之后再算
    float fixedWidth = Impl::mnAnchorFixedWidth;
    float adaptiveOffsetWidth = 0;
    int columns = 0;
    std::list<BKUnit*> adaptItems;

    // 更新垂直部分的位置
    auto loop_y = [&](BKUnit* unit, float uh, float offset = 0, int row = 0) {
        if (row == 0)
        {
            if (unit->mSizePolicy == BKUnit::SizePolicy::Fixed)
                fixedWidth += unit->mMinSize.width();
            else
            {
                fixedWidth += unit->mMinSize.width();
                adaptItems.push_back(unit);
            }
        }

        QGraphicsItem* pItem = unit->getGraphicsItem();

        if (unit->mSizePolicy == BKUnit::SizePolicy::Adaptive)        //非固定组元撑起高度
        {
            unit->mSize.setHeight(uh);
            pItem->setY(offset);
        }
        else                                                        //自适应的组元拷贝高度
        {
            unit->mSize.setHeight(unit->mMinSize.height());
            pItem->setY((uh - unit->mMinSize.height()) / 2 + offset);
        }
    };

    if (l->mType == BKCell::Type::SingleLine)
    {
        float unitHeight = aim.height();
        columns = l->mUnits.size();
        for (BKUnit* unit : l->mUnits)
            loop_y(unit, aim.height());
    }
    else if (l->mType == BKCell::Type::ListGroup)
    {
        columns = l->mTemplateUnits.size();
        int group = l->getGroupRows();
        float unitHeight = (group == 0) ? 0 : (aim.height() - l->mnGroupBtnWidth) / group - l->mnSpacing;

        if (group == 0 && l->mUnitOffset != l->mUnits.size())
            throw std::logic_error("组数与偏移不匹配");

        for (int i = l->mUnitOffset, j = 0; i < l->mUnits.size(); ++i, ++j)
        {
            BKUnit* unit = l->mUnits[i];
            int row = j / l->mTemplateUnits.size();
            loop_y(unit, unitHeight, (unitHeight + l->mnSpacing) * row , row);
        }

        for (int i = 0; i < l->mUnitOffset; ++i)
        {
            BKUnit* unit = l->mUnits[i];
            loop_y(unit, l->mnGroupBtnWidth, (unitHeight + l->mnSpacing) * group, group);
        }

        // 按钮组的宽度和X在此计算
        assert(l->mUnitOffset == 3 && "Hegemony!!");
        int btnOffset = Impl::mnAnchorFixedWidth;
        float spacerWidth = aim.width() - 2.0 * Impl::mnAnchorFixedWidth - 2.0 * l->mnGroupBtnWidth - l->mnSpacing;
        l->mUnits[0]->getGraphicsItem()->setX(btnOffset);
        l->mUnits[0]->mSize.setWidth(spacerWidth);
        btnOffset += spacerWidth;

        l->mUnits[1]->getGraphicsItem()->setX(btnOffset);
        // l->mUnits[1]->mSize = l->mUnits[1]->mMinSize;
        btnOffset += l->mnGroupBtnWidth + l->mnSpacing;

        l->mUnits[2]->getGraphicsItem()->setX(btnOffset);
        // l->mUnits[2]->mSize = l->mUnits[2]->mMinSize;

        // for(int i = 0; i < l->mUnitOffset; ++i)
        //     mpImpl->addToGroup(l->mUnits[i]->getGraphicsItem());
    }

    // 再补一个锚点
    fixedWidth += Impl::mnAnchorFixedWidth;

    // 计算均摊宽度补偿
    if (adaptItems.size())
    {
        adaptiveOffsetWidth = aim.width() - fixedWidth - (columns > 1 ? l->mnSpacing * (columns - 1) : 0);
        adaptiveOffsetWidth /= adaptItems.size();
    }

    // 更新水平部分的位置
    float x = Impl::mnAnchorFixedWidth;
    auto loop_x = [this, adaptiveOffsetWidth, &x](BKUnit* unit, int offset) {
        x += offset;

        if (unit->mSizePolicy == BKUnit::SizePolicy::Fixed)
            unit->mSize.setWidth(unit->mMinSize.width());
        else
            unit->mSize.setWidth(adaptiveOffsetWidth + unit->mMinSize.width());

        unit->getGraphicsItem()->setX(x);
        x += unit->mSize.width();
    };

    x -= l->mnSpacing;
    if (l->mType == BKCell::Type::SingleLine)
    {
        for (BKUnit* unit : l->mUnits)
            loop_x(unit, l->mnSpacing);
    }
    else if (l->mType == BKCell::Type::ListGroup)
    {
        float initX = x;
        for (int i = l->mUnitOffset, j = 1; i < l->mUnits.size(); ++i, ++j)
        {
            BKUnit* unit = l->mUnits[i];
            loop_x(unit, l->mnSpacing);

            if (j % columns == 0)
                x = initX;
        }
    }

    for (BKUnit* unit : l->mUnits)
    {
        unit->resized();

        QGraphicsItem* gi = unit->getGraphicsItem();
        if(!gi->group())
            mpImpl->addToGroup(gi);
    }

    // 更新锚点位置
    BKAnchor* la = l->mAnchorArray[0];
    BKAnchor* ra = l->mAnchorArray[1];

    float anchorY = aim.height() / 2 - BKAnchor::mnAnchorBallRadius;
    if (la)
    {
        la->setX(l->mnAnchorBallPadding);
        la->setY(anchorY);
        la->mSize = la->mMinSize;
        mpImpl->addToGroup(la->getGraphicsItem());
    }

    if (ra)
    {
        ra->setX(aim.width() - l->mnAnchorBallPadding - 2 * BKAnchor::mnAnchorBallRadius);
        ra->setY(anchorY);
        ra->mSize = ra->mMinSize;
        mpImpl->addToGroup(ra->getGraphicsItem());
    }
}


QSizeF BKCell::getTheorySize() 
{
    L_IMPL(BKCell);
    float width = 0, height = BKUnit::minUnitHeight;

    // 根据锚点设置更新最小高度
    bool hasAnchor = l->mAnchorArray[0] || l->mAnchorArray[1];
    if (hasAnchor)
        height = std::max(height, BKAnchor::mnAnchorBallRadius * 2);

    if (l->mType == BKCell::Type::SingleLine)
    {
        for (auto& item : l->mUnits)
        {
            if (height < item->mMinSize.height())
                height = item->mMinSize.height();

            width += item->mMinSize.width();
        }

        size_t column = l->mUnits.size();
        if (column > 1)
            width += l->mnSpacing * (column - 1);
    }
    else if(l->mType == BKCell::Type::ListGroup)
    {
        if (l->mTemplateUnits.size() == 0)
            throw std::logic_error("Start calc theory size, but have no template in ListGroup cell");

        for (auto& item : l->mTemplateUnits)
        {
            if (height < item->mMinSize.height())
                height = item->mMinSize.height();

            width += item->mMinSize.width();
        }

        size_t column = l->mTemplateUnits.size();
        if (column > 1)
            width += l->mnSpacing * (column - 1);                                       // 按照模板计算的满配宽度

        width = std::max(2.0f * Impl::mnGroupBtnWidth + Impl::mnSpacing, width);        // 2.0为两个按钮，弹簧可以忽略不计

        int group = l->getGroupRows();
        height =  (l->mnSpacing + height) * group + l->mnGroupBtnWidth;
    }

    //左右锚点占位
    width += 2 * l->mnAnchorFixedWidth;

    return { width, height };
}

bool BKCell::hasAnchor(BKAnchor* anchor)
{
    L_IMPL(BKCell)
    return  l->mAnchorArray[0] == anchor || l->mAnchorArray[1] == anchor;
}

BKAnchor* BKCell::getAnchor(BKAnchor::AnchorType type)
{
    L_IMPL(BKCell);

    if (type & BKAnchor::Input)
        return l->mAnchorArray[0];
    else if(type & BKAnchor::Output)
        return l->mAnchorArray[1];

    return nullptr;
}

bool BKCell::valueChanged(const QVariant& param)
{
    L_IMPL(BKCell);
    BKAnchor* ra = l->mAnchorArray[1];
    if (!ra)
        return false;

    ra->dataChanged(param);

    return true;
}

bool BKCell::exportUnitToJson(QJsonArray& obj)
{
    L_IMPL(BKCell);

    if (l->mUnits.size() == 0)
        throw std::logic_error("逻辑错误，应该已经对组元为空做了补对象的处理...");

    // 先导出锚点类型
    QJsonArray dst;
    for (BKUnit* unit : l->mUnits)
        dst.append(*unit);

    obj.append(dst);
    return true;
}

bool BKCell::updateUnitFromJson(const QJsonValue& value)
{
    L_IMPL(BKCell);

    if (l->mType == Type::ListGroup)
    {
        QJsonArray info = value.toArray();
        size_t count = info.size() - l->mUnitOffset;

        size_t templateCount = l->mTemplateUnits.size();
        if (templateCount == 0 || count % templateCount != 0)
            return false;

        int group = count / templateCount;
        push(group);
    }

    size_t count = l->mUnits.size();
    QJsonArray info = value.toArray();

    if (count == 0)
        throw std::logic_error("逻辑错误，应该已经对组元为空做了补对象的处理...");

    for (int i = 0; i < count; ++i)
    {
        BKUnit* unit = l->mUnits[i];
        unit->loadFromJson(info[i]);
    }

    return true;
}

int BKCell::getMemberRow(BKAnchor* anchor)
{
    L_IMPL(BKCell);
    if (l->mAnchorArray[0] != anchor && l->mAnchorArray[1] != anchor)
        return -1;

    return l->mRow;
}

void BKCell::bindCard(BKCard* card, int index)
{
    L_IMPL(BKCell)

    l->mRow = index;
    l->mpBindCard = card;
    for (auto anchor : l->mAnchorArray)
    {
        if (anchor)
            anchor->mpBindCard = card;
    }

    for (auto unit : l->mUnits)
        unit->mpBindCard = card;
}

QGraphicsItem* BKCell::getGraphicsItem()
{
    return mpImpl;
}

void BKCell::dispatchPositionChanged()
{
    L_IMPL(BKCell);

    for (const auto& anchor : l->mAnchorArray)
    {
        if (anchor)
            anchor->dispatchCellPositionChanged();
    }
}