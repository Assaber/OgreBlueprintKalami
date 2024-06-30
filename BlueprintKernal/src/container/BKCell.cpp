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
        // Support for allowing GraphicsItems to handle their own events
        setHandlesChildEvents(false);

        if (l != BKAnchor::None) {
            mAnchorArray[0] = new BKAnchor(l & (~BKAnchor::AnchorType::Output), mpHandle);
        }

        if (r != BKAnchor::None) {      //The output anchor comes with multiple bindings
            mAnchorArray[1] = new BKAnchor((r & (~BKAnchor::AnchorType::Input)) | BKAnchor::MultiConn, mpHandle);
        }

        if (type == Type::ListGroup)
        {
            mUnitOffset = 3;            // Try not to change the 3 here, it affects the calculation of the width

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
        // The release mechanism of Qt is not used, and it is released in batches after ungrouping
        destroyAnchorBall();
        removeAll();
        destroyUnits();
    }

public:
    void removeAll() 
    {
        for (auto& item : mUnits) {
            removeFromGroup(item->getGraphicsItem());
        }
    }

    void destroyUnits() 
    {
        for (auto& item : mUnits) {
            delete item;
        }
        mUnits.clear();
        
        for (auto& item : mTemplateUnits) {
            delete item;
        }
        mTemplateUnits.clear();
    }

    void destroyAnchorBall() 
    {
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
    std::vector<BKUnit*> mUnits;
    /* Shift of the list:
        If it is Type::ListGroup，It will contain three controls: horizontal sapcer/ + button/ - button, the value is 3
        It looks like:
              ------------------------------------------------------------------
            | Spacer | + Button  | - Button | Template Unit1 | Template Unit2 | ...
              ------------------------------------------------------------------
        If it is Type::SingleLine， the value is 0
        It looks like:
              ------------------------------------------------------------------
            | Unit1 | Unit2 | ...
              ------------------------------------------------------------------
    */
    int mUnitOffset = 0;

    std::vector<BKUnit*> mTemplateUnits;
    // The maximum number of members created from a template
    int mnMembersCountLimit = std::numeric_limits<int>::max();
    // The spacing between cells
    static constexpr int mnSpacing = 5;
    // Anchors on both sides
    std::array<BKAnchor*, 2> mAnchorArray = { nullptr, nullptr };

    static constexpr float mnAnchorBallPadding = 6;
    static constexpr float mnAnchorFixedWidth = (mnAnchorBallPadding + BKAnchor::mnAnchorBallRadius) * 2;
    // The number of rows on the card
    int mRow = -1;

    BKCell::Type mType;
    
    static constexpr int mnGroupBtnWidth = 30;

    BKCard* mpBindCard = nullptr;

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

    for (auto item : units) {
        append(item.unit, item.regist);
    }
        
    return this;
}

BKCell* BKCell::append(BKUnit* unit, bool regist/* = true*/)
{
    L_IMPL(BKCell);
    assert(l->mType == BKCell::Type::SingleLine && "This method can only be used in single-line mode");

    if (regist && l->mAnchorArray[0])
        l->mAnchorArray[0]->appendRegist(unit);

    // To be on the safe side, it's better to iterate over it to see if there are duplicate incoming ones
    // I made a mistake when I used it QAQ
    bool hasRepead = false;
    for (BKUnit* ui : l->mUnits) {
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
    for (BKUnit* unit : units) {
        l->mTemplateUnits.push_back(unit);
    }

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

                return true;        // There is no downward passing, TBD
            };

            l->mUnits.push_back(copyItem);
        }
    }

    // Triggers card layout calculations
    if (l->mpBindCard) {
        l->mpBindCard->rePackage();
    }

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

    // Triggers card layout calculations
    if (l->mpBindCard) {
        l->mpBindCard->rePackage();
    }

    return true;
}

BKCell* BKCell::setDataType(BKAnchor::AnchorType anchor, uint32_t data)
{
    L_IMPL(BKCell)
    BKAnchor* la = l->mAnchorArray[0];
    BKAnchor* ra = l->mAnchorArray[1];

    if ((anchor & BKAnchor::AnchorType::Input) && la) {
        la->setDateType(data);
    }

    if ((anchor & BKAnchor::AnchorType::Output) && ra) {
        ra->setDateType(data);
    }

    return this;
}

void BKCell::updateActualSize(const QSizeF& aim) 
{ 
    L_IMPL(BKCell);
    if(l->mType == BKCell::Type::ListGroup && l->mTemplateUnits.size() == 0)
        throw std::logic_error("Why is the exception thrown twice? It shouldn't be this way!!");

    if (l->mType == BKCell::Type::SingleLine && l->mUnits.size() == 0) 
    { 
        // Have at least one unit in a row, and use BKSpacer to as a placeholder if it is empty.
        append(BKCreator::create<BKSpacer>());
    }

    // Calculate the width of the excluded anchor first
    float fixedWidth = Impl::mnAnchorFixedWidth;
    float adaptiveOffsetWidth = 0;
    int columns = 0;
    std::list<BKUnit*> adaptItems;

    // Updates the position of the vertical section
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

        if (unit->mSizePolicy == BKUnit::SizePolicy::Adaptive)        // The height provided by the non-fixed unit
        {
            unit->mSize.setHeight(uh);
            pItem->setY(offset);
        }
        else                                                          // Fixed height, direct use
        {
            unit->mSize.setHeight(unit->mMinSize.height());
            pItem->setY((uh - unit->mMinSize.height()) / 2 + offset);
        }
    };

    if (l->mType == BKCell::Type::SingleLine)
    {
        float unitHeight = aim.height();
        columns = l->mUnits.size();
        for (BKUnit* unit : l->mUnits) {
            loop_y(unit, aim.height());
        }
    }
    else if (l->mType == BKCell::Type::ListGroup)
    {
        columns = l->mTemplateUnits.size();
        int group = l->getGroupRows();
        float unitHeight = (group == 0) ? 0 : (aim.height() - l->mnGroupBtnWidth) / group - l->mnSpacing;

        if (group == 0 && l->mUnitOffset != l->mUnits.size())
            throw std::logic_error("The number of groups does not match the offset");

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

        // Teplate control buttons' width is calculated at here
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

    // append a anchor's width
    fixedWidth += Impl::mnAnchorFixedWidth;

    // width offset
    if (adaptItems.size())
    {
        adaptiveOffsetWidth = aim.width() - fixedWidth - (columns > 1 ? l->mnSpacing * (columns - 1) : 0);
        adaptiveOffsetWidth /= adaptItems.size();
    }

    // Updates the position of the horizontal section
    float x = Impl::mnAnchorFixedWidth;
    auto loop_x = [this, adaptiveOffsetWidth, &x](BKUnit* unit, int offset) {
        x += offset;

        if (unit->mSizePolicy == BKUnit::SizePolicy::Fixed) {
            unit->mSize.setWidth(unit->mMinSize.width());
        }
        else {
            unit->mSize.setWidth(adaptiveOffsetWidth + unit->mMinSize.width());
        }

        unit->getGraphicsItem()->setX(x);
        x += unit->mSize.width();
    };

    x -= l->mnSpacing;
    if (l->mType == BKCell::Type::SingleLine)
    {
        for (BKUnit* unit : l->mUnits) {
            loop_x(unit, l->mnSpacing);
        }
    }
    else if (l->mType == BKCell::Type::ListGroup)
    {
        float initX = x;
        for (int i = l->mUnitOffset, j = 1; i < l->mUnits.size(); ++i, ++j)
        {
            BKUnit* unit = l->mUnits[i];
            loop_x(unit, l->mnSpacing);

            if (j % columns == 0) {
                x = initX;
            }
        }
    }

    for (BKUnit* unit : l->mUnits)
    {
        unit->resized();

        QGraphicsItem* gi = unit->getGraphicsItem();
        if (!gi->group()) {
            mpImpl->addToGroup(gi);
        }
    }

    // Update the location of the Anchors
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

    bool hasAnchor = l->mAnchorArray[0] || l->mAnchorArray[1];
    if (hasAnchor) { // Set the minimum height based on the anchor
        height = std::max(height, BKAnchor::mnAnchorBallRadius * 2);
    }

    if (l->mType == BKCell::Type::SingleLine)
    {
        for (auto& item : l->mUnits)
        {
            if (height < item->mMinSize.height()) {
                height = item->mMinSize.height();
            }

            width += item->mMinSize.width();
        }

        size_t column = l->mUnits.size();
        if (column > 1) {
            width += l->mnSpacing * (column - 1);
        }
    }
    else if(l->mType == BKCell::Type::ListGroup)
    {
        if (l->mTemplateUnits.size() == 0)
            throw std::logic_error("Start calc theory size, but have no template in ListGroup cell");

        for (auto& item : l->mTemplateUnits)
        {
            if (height < item->mMinSize.height()) {
                height = item->mMinSize.height();
            }

            width += item->mMinSize.width();
        }

        size_t column = l->mTemplateUnits.size();
        if (column > 1) {
            width += l->mnSpacing * (column - 1);                                       // The full width calculated according to the template
        }

        width = std::max(2.0f * Impl::mnGroupBtnWidth + Impl::mnSpacing, width);        // 2.0 is two buttons, and the spacer is negligible

        int group = l->getGroupRows();
        height =  (l->mnSpacing + height) * group + l->mnGroupBtnWidth;
    }

    // The position of the left and right anchors, even if there is no anchor they should be there, otherwise they are misaligned
    width += 2 * l->mnAnchorFixedWidth;

    return { width, height };
}

bool BKCell::hasAnchor(BKAnchor* anchor)
{
    L_IMPL(BKCell);

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
        throw std::logic_error("Logic error, the empty cell should have been appended with a spacer object...");

    QJsonArray dst;
    for (BKUnit* unit : l->mUnits) {
        dst.append(*unit);
    }

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
        throw std::logic_error("Logic error, the empty cell should have been appended with a spacer object...");

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
        if (anchor) {
            anchor->mpBindCard = card;
        }
    }

    for (auto unit : l->mUnits) {
        unit->mpBindCard = card;
    }
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
        if (anchor) {
            anchor->dispatchCellPositionChanged();
        }
    }
}