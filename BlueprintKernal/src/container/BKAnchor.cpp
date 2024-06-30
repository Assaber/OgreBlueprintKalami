#include "container/BKAnchor.h"
#include "container/BKCell.h"
#include "container/BKConnectingLine.h"
#include "BlueprintLoader.h"
#include "BKEvent.h"

#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QPainter>
#include <QDebug>

#include <stdexcept>
#include <map>

class BKAnchor::Impl
{
public:
    Impl(BKAnchor* handle, uint32_t type, BKCell* cell)
        : mpHandle(handle)
        , mAnchorType(type)
        , mpCell(cell)
    {
        mpHandle->mMinSize = { 2 * mnAnchorBallRadius , 2 * mnAnchorBallRadius };

        if ((type & AnchorType::Input) && (type & AnchorType::MultiConn))
        {
            mMultiPath.moveTo(0, mnAnchorBallRadius * 0.2f);
            mMultiPath.lineTo(mnAnchorBallRadius * 1.3f, mnAnchorBallRadius * 0.2f);
            mMultiPath.lineTo(mnAnchorBallRadius * 2, mnAnchorBallRadius);
            mMultiPath.lineTo(mnAnchorBallRadius * 1.3f, mnAnchorBallRadius * 1.8f);
            mMultiPath.lineTo(0, mnAnchorBallRadius * 1.8f);
            mMultiPath.closeSubpath();
        }
    }

    ~Impl()
    {
        while (mRegistRecord.size())
        {
            auto item = mRegistRecord.begin();
            auto otherAnchor = item->first;

            BKConnectingLine* line = item->second;
            StandAloneUnit* u = dynamic_cast<StandAloneUnit*>(line);
            auto view = getInnerView(u->getBindItem()->scene());
            if (view) {
                view->destroyUnit(line);
            }

            otherAnchor->update();
        }           
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);

public:
    void setDateType(uint32_t type);

public:
    BlueprintLoader* getInnerView(QGraphicsScene* scene = nullptr);

public:
    void appendRegist(BKAnchor* anchor, BKConnectingLine* path);
    void removeRegist(BKAnchor* anchor);
    void appendRegist(BKUnit* unit);
    void removeRegist(BKUnit* unit);

public:
    BKAnchor* mpHandle = nullptr;
    uint32_t mAnchorType = AnchorType::None;
    uint32_t mDataType = DataType::Default;

    std::map<BKAnchor*, BKConnectingLine*> mRegistRecord;
    std::set<BKUnit*> mRegistUnitSet;

    bool mbConnect = false;
    bool mbReadyEstablishConn = false;

    QColor mColor = QColor(0xFFFF8000);
    QBrush mFilledBrush = mColor;
    QPen mBorderPen = mColor;


    static std::map<uint32_t, QColor> mDataType2Color;
    // The start point of the connect line
    QPointF mConnectBegin;
    // Master cell
    BKCell* mpCell = nullptr;
    // Different from the circular shape of the normal anchor point, the shape unique to the anchor point is input
    QPainterPath mMultiPath;
};

// This color scheme will make people feel happyヽ(°▽°)ノ, After all, it's the color of ￥
// Thanks Sister Meng for the inspiration provided by the share looooooooong long ago
std::map<uint32_t, QColor> BKAnchor::Impl::mDataType2Color = {
    { BKAnchor::DataType::Default,      0xFFFF8000 },           // My favorite color
    { BKAnchor::DataType::Boolean,      0xFFA28C88 },           // 0.1      
    { BKAnchor::DataType::Short,        0xFF708E98 },           // 0.2
    { BKAnchor::DataType::Integer,      0xFF8B6773 },           // 0.5
    { BKAnchor::DataType::String,       0xFF89916B },           // 1
    { BKAnchor::DataType::Float,        0xFF846992 },           // 5
    { BKAnchor::DataType::Double,       0xFF435C74 },           // 10
    { BKAnchor::DataType::VecInteger,   0xFF886349 },           // 20
    { BKAnchor::DataType::VecFloat,     0xFF4F9C83 },           // 50
    { BKAnchor::DataType::Custom,       0xFFBF6074 },           // 100
};

BKUnit* BKAnchor::copy()
{
    L_IMPL(BKAnchor);
    BKAnchor* target = BKCreator::create<BKAnchor>(l->mAnchorType, l->mpCell);

    BKAnchor::Impl* dstImpl = target->mpImpl;
    dstImpl->mColor = l->mColor;
    dstImpl->mFilledBrush = l->mFilledBrush;
    dstImpl->mBorderPen = l->mBorderPen;
    _copyBasicAttributeTo(target);
    return target;
}

bool BKAnchor::loadFromJson(const QJsonValue& val)
{
    return true;
}

QVariant BKAnchor::data()
{
    return true;
}

BKAnchor::operator QJsonValue() const
{
    return true;
}

BKAnchor::BKAnchor(uint32_t type, BKCell* cell)
    : super()
    , mpImpl(new Impl(this, type, cell))
{
    mSizePolicy = SizePolicy::Fixed;
}

BKAnchor::~BKAnchor()
{
    delete mpImpl;
    mpImpl = nullptr;
}

BKAnchor* BKAnchor::setDateType(uint32_t type)
{
    mpImpl->setDateType(type);
    return this;
}

uint32_t BKAnchor::getAnchorType()
{
    return mpImpl->mAnchorType;
}

uint32_t BKAnchor::getDataType()
{
    return mpImpl->mDataType;
}

bool BKAnchor::hasConnected()
{
    return mpImpl->mbConnect;
}

void BKAnchor::appendRegist(BKAnchor* anchor, BKConnectingLine* path/* = nullptr*/) { mpImpl->appendRegist(anchor, path); }
void BKAnchor::Impl::appendRegist(BKAnchor* anchor, BKConnectingLine* path)
{
    mRegistRecord.insert({ anchor, path });
    mbConnect = mRegistRecord.size();
}

void BKAnchor::appendRegist(BKUnit* unit) { mpImpl->appendRegist(unit); }
void BKAnchor::Impl::appendRegist(BKUnit* unit)
{
    mRegistUnitSet.insert(unit);
}

void BKAnchor::removeRegist(BKAnchor* anchor) { mpImpl->removeRegist(anchor); }
void BKAnchor::Impl::removeRegist(BKAnchor* anchor)
{
    auto itor = mRegistRecord.find(anchor);
    if (itor == mRegistRecord.end())
        return;

    mRegistRecord.erase(itor);
    mbConnect = mRegistRecord.size();
}


void BKAnchor::removeRegist(BKUnit* unit) { mpImpl->removeRegist(unit); }

bool BKAnchor::redirectToCard()
{
    L_IMPL(BKAnchor);

    if (!(l->mAnchorType & BKAnchor::Output))
        return false;

    auto units = getRegistUnits();
    for (auto unit : units)
        removeRegist(unit);

    return true;
}

void BKAnchor::registDataType(uint32_t type, const QColor& color /*= QColor(qrand() % 255, qrand() % 255, qrand() % 255)*/)
{
    auto itor = Impl::mDataType2Color.find(type);
    if (itor != Impl::mDataType2Color.end())
    {
        if (type > BKAnchor::Custom) 
        {
            QString msg = QString("The registration type is incorrect, and the same custom type is registered repeatedly<%1>").arg(type);
            throw std::logic_error(msg.toStdString());
        }
        else {
            itor->second = color;
        }
    }
    else {
        Impl::mDataType2Color[type] = color;
    }
}

QColor BKAnchor::getColorByDataType(uint32_t type)
{
    auto itor = Impl::mDataType2Color.find(type);
    return itor == Impl::mDataType2Color.end() ? Impl::mDataType2Color[BKAnchor::DataType::Default] : itor->second;
}

void BKAnchor::dispatchCellPositionChanged()
{
    for (auto item : mpImpl->mRegistRecord)
        item.second->updateByBind();
}

std::vector<BKAnchor*> BKAnchor::getRegistAnchors()
{
    L_IMPL(BKAnchor)
    std::vector<BKAnchor*> ret;

    for (auto item : l->mRegistRecord)
        ret.push_back(item.first);

    return ret;
}

std::set<BKUnit*> BKAnchor::getRegistUnits()
{
    return mpImpl->mRegistUnitSet;
}

BKCell* BKAnchor::getCell()
{
    return mpImpl->mpCell;
}

void BKAnchor::Impl::removeRegist(BKUnit* unit)
{
    auto itor = mRegistUnitSet.find(unit);
    if ( itor != mRegistUnitSet.end())
        mRegistUnitSet.erase(itor);
}

bool BKAnchor::sceneEvent(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::GraphicsSceneMousePress:
    {
        mpImpl->mbReadyEstablishConn = true;
        auto view = mpImpl->getInnerView();
        if (view)
        {
            mpImpl->mConnectBegin = mapToScene(boundingRect().center());
            PreConnLineEvent pe;
            pe.setType(PreConnLineEvent::EventType::Pressed)
                .setColorPtr(&mpImpl->mColor)
                .setBeginPoint(mpImpl->mConnectBegin);
            qApp->sendEvent(view, &pe);
            update();
        }
        return true;
    }
    break;

    case QEvent::GraphicsSceneMouseMove:
    {
        auto view = mpImpl->getInnerView();
        if (view)
        {
            PreConnLineEvent pe;
            pe.setType(PreConnLineEvent::EventType::Move)
                .setColorPtr(&mpImpl->mColor)
                .setBeginPoint(mpImpl->mConnectBegin)
                .setEndPoint(dynamic_cast<QGraphicsSceneMouseEvent*>(event)->scenePos());
            qApp->sendEvent(view, &pe);
            update();
        }
    }
    break;

    case QEvent::GraphicsSceneMouseRelease:
    {
        auto view = mpImpl->getInnerView();
        if (view)
        {
            PreConnLineEvent pe;
            pe.setType(PreConnLineEvent::EventType::Released)
                .setColorPtr(&mpImpl->mColor)
                .setBeginPoint(mpImpl->mConnectBegin)
                .setEndPoint(dynamic_cast<QGraphicsSceneMouseEvent*>(event)->scenePos())
                .setSender(this);
            qApp->sendEvent(view, &pe);
        }

        mpImpl->mbReadyEstablishConn = false;
        update();
        return true;
    }
    break;

    }

    return QGraphicsItem::sceneEvent(event);
}

void BKAnchor::dataChanged(const QVariant& data)
{
    L_IMPL(BKAnchor)

    if (l->mAnchorType & AnchorType::Input) 
    {
        for (BKUnit* item : l->mRegistUnitSet) {
            item->dataChanged(data);
        }
    }
    else if (l->mAnchorType & AnchorType::Output) 
    {
        for (auto item : l->mRegistRecord) {
            item.first->dataChanged(data);
        }
    }
}

void BKAnchor::dataChanged()
{
    L_IMPL(BKAnchor);

    if (l->mAnchorType & AnchorType::Input)
    {
        for (BKUnit* item : l->mRegistUnitSet) {
            item->dataChanged();
        }
    }
    else if (l->mAnchorType & AnchorType::Output)
    {
        for (auto item : l->mRegistRecord) {
            item.first->dataChanged();
        }
    }
}

int BKAnchor::getBindOutputData(std::vector<QVariant>& rec)
{
    L_IMPL(BKAnchor);

    if (l->mAnchorType & AnchorType::Output)
        return -1;

    rec.clear();
    for (auto other : l->mRegistRecord)
    {
        auto ol = other.first->mpImpl;
        auto card = other.first->mpBindCard;

        if (ol->mRegistUnitSet.size() == 0) {           // Only output anchor bound to card can get the value of card
            rec.push_back(card->getCurrentCardValue());
        }
    }

    return rec.size();
}

void BKAnchor::Impl::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
{
    static_cast<void*>(widget);

    painter->save();
    {
        painter->setPen(mBorderPen);
        if (mbConnect || mbReadyEstablishConn)
            painter->setBrush(mFilledBrush);
        else
            painter->setBrush(Qt::NoBrush);

        if((mAnchorType & AnchorType::Input) && (mAnchorType & AnchorType::MultiConn))
            painter->drawPath(mMultiPath);
        else
            painter->drawEllipse({ mnAnchorBallRadius, mnAnchorBallRadius }, mnAnchorBallRadius, mnAnchorBallRadius);

        painter->restore();
    }
}

void BKAnchor::Impl::setDateType(uint32_t type)
{
    mDataType = type;
    auto itor = mDataType2Color.find(mDataType);
    if ( itor != mDataType2Color.end())
    {
        mColor = itor->second;
        mFilledBrush = QBrush(mColor);
        mBorderPen = QPen(mColor);
    }
}

BlueprintLoader* BKAnchor::Impl::getInnerView(QGraphicsScene* s)
{
    BlueprintLoader* ret = nullptr;
    QGraphicsScene* scene = s ? s : mpHandle->scene();

    do 
    {
        if (!scene)
            break;

        for (auto view : scene->views())
        {
            if (ret = dynamic_cast<BlueprintLoader*>(view))
                break;
        }

    } while (false);

    return ret;
}

bool BKAnchor::inSameCell(BKAnchor* anchor) 
{ 
    L_IMPL(BKAnchor);

    if (!l->mpCell || anchor == this) // Ask and nod XD
        return true;

    return l->mpCell->hasAnchor(anchor);
}

bool BKAnchor::hasRegisted(BKAnchor* other)
{
    L_IMPL(BKAnchor);

    return l->mRegistRecord.find(other) != l->mRegistRecord.end();
}

QGraphicsItem* BKAnchor::getGraphicsItem()
{
    return this;
}

QRectF BKAnchor::boundingRect() const
{
    return { 0, 0, mSize.width(), mSize.height() };
}

void BKAnchor::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
{
    mpImpl->paint(painter, option, widget);
}
