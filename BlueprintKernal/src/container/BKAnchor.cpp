#include "container/BKAnchor.h"
#include <map>
#include <QPainter>
#include <set>
#include <QDebug>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include "BlueprintLoader.h"
#include "container/BKConnectingLine.h"
#include <QApplication>

class BKAnchor::Impl
{
public:
    Impl(BKAnchor* handle, AnchorType type, BKCell* cell)
        : mpHandle(handle)
        , mAnchorType(type)
        , mpCell(cell)
    {
        mpHandle->mMinSize = { 2 * mnAnchorBallRadius , 2 * mnAnchorBallRadius };
    }

    ~Impl()
    {
        qDebug() << "锚点释放";
        for (auto item : mRegistRecord)
        {
            // 不用担心移除注册的事，连接线会Do it
            // removeRegist(item.first);

            BKConnectingLine* line = item.second;
            StandAloneUnit* u = dynamic_cast<StandAloneUnit*>(line);
            auto view = getInnerView(u->getBindItem()->scene());
            if (view)
            {
                qDebug() << "删除连接线";
                view->destroyUnit(line);
            }

            item.first->update();
        }
           
    }


    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);
public:
    void setDateType(DataType type);
    void setBezierOffset(int offset);
    bool inSameCell(BKAnchor* anchor);

public:
    BlueprintLoader* getInnerView(QGraphicsScene* scene = nullptr);

public:
    void appendRegist(BKAnchor* anchor, BKConnectingLine* path);
    void removeRegist(BKAnchor* anchor);
    void appendRegist(BKUnit* unit);
    void removeRegist(BKUnit* unit);

public:
    BKAnchor* mpHandle = nullptr;
    AnchorType mAnchorType = AnchorType::None;
    DataType mDataType = DataType::Default;

    std::map<BKAnchor*, BKConnectingLine*> mRegistRecord;
    std::set<BKUnit*> mRegistUnitSet;

    // 已经存在连接
    bool mbConnect = false;
    // 准备建立连接
    bool mbReadyEstablishConn = false;
    // 锚点配色
    QColor mColor = QColor(0xFFFF8000);
    // 填充笔刷
    QBrush mFilledBrush = mColor;
    // 边界画笔
    QPen mBorderPen = mColor;
    // 数据类型到颜色的映射
    static const std::map<DataType, QColor> mDataType2Color;
    // 准备线的其实连接点
    QPointF mConnectBegin;
    // 锚点偏移，为了让连接线看着平行，逻辑上基于最上面的锚点做出偏移
    int mAnchorOffset = 0;
    // 包裹锚点的组元
    BKCell* mpCell = nullptr;
};

// 这个配色会让人觉得踏实，不要问为什么，毕竟取自人民币配色√
// 感谢萌姐好几年前朋友圈提供的灵感
const std::map<BKAnchor::DataType, QColor> BKAnchor::Impl::mDataType2Color = {
    { BKAnchor::DataType::Default,  0xFFFF8000 },
    { BKAnchor::DataType::Boolean,  0xFFA28C88 },
    { BKAnchor::DataType::Short,    0xFF708E98 },
    { BKAnchor::DataType::Integer,  0xFF8B6773 },
    { BKAnchor::DataType::String,   0xFF89916B },
    { BKAnchor::DataType::Float,    0xFF846992 },
    { BKAnchor::DataType::Double,   0xFF435C74 },
    { BKAnchor::DataType::Custom,   0xFFFF8000 },
};


BKAnchor::BKAnchor(AnchorType type, BKCell* cell)
    : super()
    , mpImpl(new Impl(this, type, cell))
{
    mSizePolicy = SizePolicy::Fixed;
}

BKAnchor::~BKAnchor()
{
    //释放连接线
    delete mpImpl;
    mpImpl = nullptr;
}

BKAnchor* BKAnchor::setDateType(DataType type)
{
    mpImpl->setDateType(type);
    return this;
}

BKAnchor* BKAnchor::setBezierOffset(int offset)
{
    mpImpl->setBezierOffset(offset);
    return this;
}


BKAnchor::AnchorType BKAnchor::getAnchorType()
{
    return mpImpl->mAnchorType;
}

BKAnchor::DataType BKAnchor::getDataType()
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
    // 只有可能Unit向输入锚点注入
    if (mAnchorType == AnchorType::Input)
        mRegistUnitSet.insert(unit);
    else
        qWarning() << "注册单元关联失败";
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

void BKAnchor::dispatchCellPositionChanged()
{
    for (auto item : mpImpl->mRegistRecord)
        item.second->updateByBind();
}

void BKAnchor::Impl::removeRegist(BKUnit* unit)
{
    if (auto itor = mRegistUnitSet.find(unit); itor != mRegistUnitSet.end())
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

void BKAnchor::Impl::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
{
    static_cast<void*>(widget);

    // 绘制文字
    painter->save();
    {
        painter->setPen(mBorderPen);
        if (mbConnect || mbReadyEstablishConn)
            painter->setBrush(mFilledBrush);
        else
            painter->setBrush(Qt::NoBrush);

        painter->drawEllipse({ mnAnchorBallRadius, mnAnchorBallRadius }, mnAnchorBallRadius, mnAnchorBallRadius);
        painter->restore();
    }
}

void BKAnchor::Impl::setDateType(DataType type)
{
    mDataType = type;

    if (auto itor = mDataType2Color.find(mDataType); itor != mDataType2Color.end())
    {
        mColor = itor->second;
        mFilledBrush = QBrush(mColor);
        mBorderPen = QPen(mColor);
    }
}

void BKAnchor::Impl::setBezierOffset(int offset)
{
    mAnchorOffset = offset;
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

bool BKAnchor::inSameCell(BKAnchor* anchor) { return mpImpl->inSameCell(anchor); }

bool BKAnchor::hasRegisted(BKAnchor* other)
{
    return mpImpl->mRegistRecord.find(other) != mpImpl->mRegistRecord.end();
}

bool BKAnchor::Impl::inSameCell(BKAnchor* anchor)
{
    if (!mpCell || anchor == mpHandle)        //休怪无情
        return true;

    return mpCell->hasAnchor(anchor);
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
