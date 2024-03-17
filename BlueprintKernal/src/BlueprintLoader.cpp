#include "BlueprintLoader.h"
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QDebug>
#include "container/BKConnectingLine.h"
#include "BKEvent.h"

class BlueprintLoader::Impl
{
public:
    Impl(BlueprintLoader* loader, QGraphicsScene* scene)
        : mpView(loader)
        , mpScene(scene)
    {

    }

    ~Impl()
    {
        /* 只释放卡片，卡片里面的成分有、复杂，需要按层释放
            - 卡片间的连接线会在卡片内部进行释放
            - 预连接线需要手动释放
        */
        for (auto unit : mpView->mUnitsRecord)
        {
            int unitType = unit.first->data(StandAloneUnitInUserData).toInt();
            if (static_cast<StandAloneUnit::Type>(unitType) == StandAloneUnit::Type::Card)
                delete unit.second;
            else if (unit.second == mpReadyLine)
            {
                mpScene->removeItem(unit.first);
                delete unit.second;
            }
        }

        mpView->mUnitsRecord.clear();
    }

public:
    void init();
    void scale(QWheelEvent* event);
    bool PreConnectLineEvent(PreConnLineEvent* event);
    void deleteSelectedBKObject();
    void destroyUnit(QGraphicsItem* item);
    void updateTopmostCardItem(QGraphicsItem* card);

private:
    void initScene();
    void initView();
    void updateBgBrush();

public:
    // 场景视口
    BlueprintLoader* mpView;
    // 全场景
    QGraphicsScene* mpScene = nullptr;
    // 中心点
    QPointF mCenter = { 0, 0 };
    // 场景范围
    QRectF mSceneRect = { -1500.0f, -1500.0f, 3000.0f, 3000.0f };
    //视口缩放
    float mnScaleRatio = 1.0f;
    // 连接线前身
    BKConnectingLine* mpReadyLine = nullptr;
    // 最后一个活跃的卡片
    QGraphicsItem* mpLastActiveCard = nullptr;
};

void BlueprintLoader::Impl::init()
{
    initScene();
    initView();

    // 初始化连接线
    mpView->createUnit<BKConnectingLine>();
    int count = 0;
    for (auto item : mpView->mUnitsRecord)
    {
        if (item.second->getUnitType() == StandAloneUnit::Type::ConnectingLine)
        {
            mpReadyLine = dynamic_cast<BKConnectingLine*>(item.second);
            ++count;
        }
    }

    if (count != 1)
        throw std::exception("应该就一条连接线才对，是谁插队！");
}

void BlueprintLoader::Impl::initScene()
{
    mpScene->setItemIndexMethod(QGraphicsScene::NoIndex);        //规避removeItem再delete后崩溃
    mpScene->installEventFilter(mpView);
    mpScene->setSceneRect(mSceneRect);
    // updateBgBrush();
}

void BlueprintLoader::Impl::initView()
{
    mpView->setScene(mpScene);
    mpView->setSceneRect(mSceneRect);
    //干掉拖拽残影
    mpView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //永久隐藏滑动条
    mpView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mpView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //视口居中
    mpView->centerOn(mCenter);
    //反锯齿
    mpView->setRenderHint(QPainter::Antialiasing);
    //拖拽设置
    mpView->setDragMode(QGraphicsView::ScrollHandDrag);
    mpView->viewport()->setCursor(Qt::ArrowCursor);
    //设置接收拖拽
    mpView->setAcceptDrops(true);
}

void BlueprintLoader::Impl::updateBgBrush()
{
    const int r = 3;                //每个锚点的半径
    const int delta = 10;            //每delta * 2r间隔绘制一个锚点

    const int width = r * 2 * (delta + 1);
    QImage anchor(width, width, QImage::Format::Format_RGB32);
    anchor.fill(Qt::white);
    QPainter painter(&anchor);
    {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawEllipse(QPoint(width / 2, width / 2), r, r);
        painter.end();
    }

    mpScene->setBackgroundBrush(QBrush(anchor));
}

void BlueprintLoader::Impl::scale(QWheelEvent* event)
{
    auto delta = event->delta() > 0 ? 1.2f : 0.8f;
    if (mnScaleRatio < 0.5f && delta < 1.0f || mnScaleRatio > 5.0f && delta > 1.0f)
        return;

    mpView->scale(delta, delta);
    mnScaleRatio *= delta;
}

bool BlueprintLoader::Impl::PreConnectLineEvent(PreConnLineEvent* event)
{
    if (!event)
        return false;

    switch (event->type)
    {
    case PreConnLineEvent::EventType::Pressed:
        mpReadyLine->setColor(event->color ? *event->color : Qt::black)
            .setPainterPath(event->begin)
            .setVisible(true);
        break;
    case PreConnLineEvent::EventType::Move:
        mpReadyLine->setPainterPath(event->begin, event->end);
        break;
    case PreConnLineEvent::EventType::Released:
        if (auto sender = dynamic_cast<BKAnchor*>(event->sender))
        {
            for (auto item : mpScene->items(event->end))
            {
                auto anchor = dynamic_cast<BKAnchor*>(item);
                if (!anchor)                   //释放对象也为锚点
                    continue;

                if (anchor->getAnchorType() == sender->getAnchorType())             // 锚点不能为同种类型
                    continue;

                if (anchor->getDataType() != sender->getDataType())                 // 锚点数据类型必须相同
                    continue;
                
                auto input = sender->getAnchorType() == BKAnchor::Input ? sender : anchor;
                if (input->hasConnected())                                          // 禁止多个输出对应一个输入
                    continue;

                if(anchor->hasRegisted(sender))
                    continue;

                mpView->createUnit<BKConnectingLine>(event->color ? *event->color : Qt::black, anchor, sender);
                break;
            }
        }
        mpReadyLine->setVisible(false);
        break;
    default:
        break;
    }

    return true;
}

void BlueprintLoader::Impl::deleteSelectedBKObject()
{
    for (auto selected : mpView->mScene.selectedItems())
    {
        StandAloneUnit::Type type = static_cast<StandAloneUnit::Type>(selected->data(StandAloneUnitInUserData).toInt());
        switch (type)
        {
        case StandAloneUnit::Type::Card:
        case StandAloneUnit::Type::ConnectingLine:
            destroyUnit(selected);
            break;
        default:
            mpScene->removeItem(selected);
            delete selected;
            break;
        }
    }
}

void BlueprintLoader::Impl::destroyUnit(QGraphicsItem* item)
{
    mpScene->removeItem(item);
    if (item == mpLastActiveCard)
        mpLastActiveCard = nullptr;

    auto itor = mpView->mUnitsRecord.find(item);
    if (itor == mpView->mUnitsRecord.end())
        return;
    StandAloneUnit* unit = itor->second;
    mpView->mUnitsRecord.erase(itor);

    delete unit;
}

void BlueprintLoader::Impl::updateTopmostCardItem(QGraphicsItem* card)
{
    if (card == mpLastActiveCard)
        return;

    card->setZValue(1.5f);
    card->update();

    if (mpLastActiveCard)        // 回归初心
    {
        mpLastActiveCard->setZValue(0);
        card->update();
    }

    mpLastActiveCard = card;
}

void BlueprintLoader::destroyUnit(StandAloneUnit* unit)
{
    auto item = unit->getBindItem();
    mpImpl->destroyUnit(item);
}

BlueprintLoader::BlueprintLoader(QWidget* parent /*= nullptr*/)
    : QGraphicsView(parent)
    , mpImpl(new Impl(this, &mScene))
{
    mpImpl->init();
}

BlueprintLoader::~BlueprintLoader()
{
    delete mpImpl;
}

bool BlueprintLoader::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == UpdateTopmostCardEvent)
    {
        mpImpl->updateTopmostCardItem(dynamic_cast<TopmostCardEvent*>(event)->mpCard);
        return true;
    }

    return QGraphicsView::eventFilter(obj, event);
}

void BlueprintLoader::wheelEvent(QWheelEvent* event)
{
    mpImpl->scale(event);
    event->accept();
}

void BlueprintLoader::mousePressEvent(QMouseEvent* event)
{
    L_IMPL(BlueprintLoader)
    QGraphicsView::mousePressEvent(event);

    for (auto item : mScene.selectedItems())
    {
        auto itor = mUnitsRecord.find(item);
        if (itor != mUnitsRecord.end() 
            && itor->second->getUnitType() == StandAloneUnit::Type::Card)
        {
            l->updateTopmostCardItem(itor->first);
        }
    }
}

void BlueprintLoader::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void BlueprintLoader::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete)
    {
        // 删除卡片或者连线
        mpImpl->deleteSelectedBKObject();
    }


    QGraphicsView::keyPressEvent(event);
}

bool BlueprintLoader::event(QEvent* event)
{
    if (event->type() == AnchorMouseEvent)
        return mpImpl->PreConnectLineEvent(dynamic_cast<PreConnLineEvent*>(event));
        
    return QGraphicsView::event(event);
}