#include "BlueprintLoader.h"
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QDebug>
#include "container/BKConnectingLine.h"
#include "BKEvent.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QScrollBar>
#include <QJsonArray>
#include "CardFilterComboBox.h"

class BlueprintLoader::Impl
{
public:
    Impl(BlueprintLoader* loader, QGraphicsScene* scene)
        : mpView(loader)
        , mpScene(scene)
        , mpCreatorComboBox(new CardFilterComboBox(loader))
    {
    }

    ~Impl()
    {
        destroyAllItems();

        /* Only release the card, the members in the card are somewhat complicated, and need to be released by layer
            - The connecting lines between the cards are released inside the cards
            - The pre-connected line needs to be released manually
        */
        delete mpReadyLine;
        mpReadyLine = nullptr;

        mpView->mUnitsRecord.clear();
    }

public:
    void init();
    void scale(QWheelEvent* event);
    bool PreConnectLineEvent(PreConnLineEvent* event);
    void deleteSelectedBKObject();
    void destroyUnit(QGraphicsItem* item);
    void updateTopmostCardItem(QGraphicsItem* card);
    void destroyAllItems();

private:
    void initScene();
    void initView();
    void updateBgBrush();

public:
    BlueprintLoader* mpView;
    QGraphicsScene* mpScene = nullptr;
    
    QPointF mCenter = { 0, 0 };
    
    QRectF mSceneRect = { -1500.0f, -1500.0f, 3000.0f, 3000.0f };
    
    float mnScaleRatio = 1.0f;
    // Pre-connected line. A connection line is not actually created until two endpoints are confirmed, and until then it is represented by this model worker
    BKConnectingLine* mpReadyLine = nullptr;

    QGraphicsItem* mpLastActiveCard = nullptr;
    
    CardFilterComboBox* mpCreatorComboBox  = nullptr;
};

void BlueprintLoader::Impl::init()
{
    initScene();
    initView();

    // Init the pre-connected line
    mpReadyLine = mpView->_createUnit<BKConnectingLine>();

    mpCreatorComboBox->hide();
}

void BlueprintLoader::Impl::initScene()
{
    mpScene->setItemIndexMethod(QGraphicsScene::NoIndex);        //Avoid the crash after removeItem && delete
    mpScene->installEventFilter(mpView);
    mpScene->setSceneRect(mSceneRect);
    // updateBgBrush();
}

void BlueprintLoader::Impl::initView()
{
    mpView->setScene(mpScene);
    mpView->setSceneRect(mSceneRect);

    mpView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    mpView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mpView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mpView->centerOn(mCenter);

    mpView->setRenderHint(QPainter::Antialiasing);

    mpView->setDragMode(QGraphicsView::ScrollHandDrag);
    mpView->viewport()->setCursor(Qt::ArrowCursor);
    // It doesn't seem to be useful at the moment
    mpView->setAcceptDrops(true);
}

void BlueprintLoader::Impl::updateBgBrush()
{
    const int r = 3;
    const int delta = 10;

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
                if (!anchor)                                                                                        //The dst target should also be an anchor
                    continue;

                if (anchor->getAnchorType() == sender->getAnchorType())                                             // Anchors should have different types
                    continue;
                    
                if (anchor->mpBindCard == sender->mpBindCard)                                                       // It is forbidden to entertain oneself
                    continue;

                if (anchor->getDataType() != sender->getDataType())                                                 // Anchors should have same data type
                    continue;
                
                auto input = (sender->getAnchorType() & BKAnchor::Input) ? sender : anchor;
                if (input->hasConnected() && (input->getAnchorType() & BKAnchor::AnchorType::MultiConn) == 0)       // Multiple outputs for one input are prohibited，unless, of course, you're a VIP
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
    StandAloneUnit* unit = itor->second.item;
    mpView->mUnitsRecord.erase(itor);

    delete unit;
}

void BlueprintLoader::Impl::updateTopmostCardItem(QGraphicsItem* card)
{
    if (card == mpLastActiveCard)
        return;

    card->setZValue(1.5f);
    card->update();

    if (mpLastActiveCard)        // Revert previous privileges
    {
        mpLastActiveCard->setZValue(0);
        card->update();
    }

    mpLastActiveCard = card;
}

void BlueprintLoader::Impl::destroyAllItems()
{
    auto& record = mpView->mUnitsRecord;

    auto itor = record.begin();
    while (itor != record.end())
    {
        int unitType = itor->first->data(StandAloneUnitInUserData).toInt();
        auto item = itor->second.item;

        if (static_cast<StandAloneUnit::Type>(unitType) == StandAloneUnit::Type::Card)
        {
            delete item;
            itor = record.erase(itor);
        }
        else {
            ++itor;
        }
    }
}

CardFilterComboBox* BlueprintLoader::getFilterMenuPtr()
{
    return mpImpl->mpCreatorComboBox;
}

void BlueprintLoader::destroyUnit(StandAloneUnit* unit)
{
    auto item = unit->getBindItem();
    mpImpl->destroyUnit(item);
}

void BlueprintLoader::exportSceneToJson(const QString& path)
{
    L_IMPL(BlueprintLoader);

    QFile file(path);
    if (file.exists()) {
        file.remove();
    }

    if (!file.open(QIODevice::ReadWrite))
        return;

    QJsonDocument doc;
    QJsonObject scene;
    QJsonArray cards, lines;
    std::map<BKCard*, int> card2Index;
    for (const auto& item : mUnitsRecord)
    {
        auto type = static_cast<StandAloneUnit::Type>(item.first->data(StandAloneUnitInUserData).toInt());
        if( type == StandAloneUnit::Type::Card)
        {
            auto card = dynamic_cast<BKCard*>(item.second.item);

            QJsonArray obj(std::move(card->exportToJson()));
            if (obj.isEmpty())
                continue;

            QJsonObject cardObject{
                {"index", cards.size() },
                {"name", item.second.name },
                {"x", item.first->x()},
                {"y", item.first->y()},
                {"rows", obj}
            };

            cards.append(cardObject);
            card2Index[card] = cardObject["index"].toInt();
        }
    };

    for (const auto& item : mUnitsRecord)
    {
        auto type = static_cast<StandAloneUnit::Type>(item.first->data(StandAloneUnitInUserData).toInt());
        if( type == StandAloneUnit::Type::ConnectingLine)
        {
            auto line = dynamic_cast<BKConnectingLine*>(item.second.item);
            BKConnectingLine::BasicInfo info;
            if (!line->getBasicInfo(info))
                continue;

            auto lItor = card2Index.find(info.lCard);
            int lIndex = (lItor == card2Index.end()) ? -1 : lItor->second;

            auto rItor = card2Index.find(info.rCard);
            int rIndex = (rItor == card2Index.end()) ? -1 : rItor->second;

            if (lIndex < 0 || rIndex < 0)
                continue;

            QJsonObject lineObject{
                {"card1", lIndex },
                {"card2", rIndex },
                {"row1", info.lIndex },
                {"row2", info.rIndex}
            };
            lines.append(lineObject);
        }
    }

    scene.insert("card", cards);
    scene.insert("connect", lines);
    doc.setObject(scene);
    file.write(doc.toJson());
    file.flush();

    file.close();
}

bool BlueprintLoader::loadSceneFromJson(const QString& path)
{
    L_IMPL(BlueprintLoader);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();

    if (error.error != QJsonParseError::NoError)
        return false;

    // clear all items
    l->destroyAllItems();

    auto cardsObject = doc["card"];
    if (cardsObject.isNull())
        return false;

    auto cards = cardsObject.toArray();
    std::map<int, BKCard*> record;
    for (const auto& item : cards)
    {
        auto card = item.toObject();
        QString qname = card["name"].toString();
        auto creator = l->mpCreatorComboBox->getCreator(qname, "");
        if (!creator)
            continue;

        auto newItem = creator(this);
        newItem->loadFromJson(card["rows"].toArray() , { card["x"].toInt(), card["y"].toInt() });

        record[card["index"].toInt()] = newItem;
    }

    auto linesObject = doc["connect"];
    auto get_card = [&](int index, int row, BKAnchor::AnchorType type) -> BKAnchor* {
        auto itor = record.find(index);
        if (itor == record.end())
        {
            qWarning() << "W-H-E-R-E I-S M-Y C-A-R-D!!!!! M-I-N-E" << index;
            return nullptr;
        }
            

        auto card = itor->second;
        return card->getRowAnchor(row, type);
    };

    if (!linesObject.isNull())
    {
        for (const auto& item : linesObject.toArray())
        {
            auto connect = item.toObject();
            auto lAnchor = get_card(connect["card1"].toInt(), connect["row1"].toInt(), BKAnchor::AnchorType::Input);
            auto rAnchor = get_card(connect["card2"].toInt(), connect["row2"].toInt(), BKAnchor::AnchorType::Output);

            if (!lAnchor || !rAnchor) {
                continue;
            }

            createUnit<BKConnectingLine>(BKAnchor::getColorByDataType(lAnchor->getDataType()), lAnchor, rAnchor);
        }
    }

    return true;
}

void BlueprintLoader::setDarling(BKCard* card)
{
    L_IMPL(BlueprintLoader);

    if (!card || !card->getBindItem())
        return; 

    l->updateTopmostCardItem(card->getBindItem());
}

BlueprintLoader::BlueprintLoader(QWidget* parent /*= nullptr*/)
    : QGraphicsView(parent)
    , mpImpl(new Impl(this, &mScene))
{
    mpImpl->init();
    verticalScrollBar()->setEnabled(false);
    horizontalScrollBar()->setEnabled(false);
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
    QGraphicsView::wheelEvent(event);

    if (!event->isAccepted())
    {
        mpImpl->scale(event);
        event->accept();
    }
}

void BlueprintLoader::mousePressEvent(QMouseEvent* event)
{
    L_IMPL(BlueprintLoader)
    QGraphicsView::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        for (auto item : mScene.selectedItems())
        {
            auto itor = mUnitsRecord.find(item);
            if (itor != mUnitsRecord.end()
                && itor->second.item->getUnitType() == StandAloneUnit::Type::Card)
            {
                l->updateTopmostCardItem(itor->first);
            }
        }

        if (l->mpCreatorComboBox->isActiveWindow()) {
            l->mpCreatorComboBox->hide();
        }
    }
}

void BlueprintLoader::mouseReleaseEvent(QMouseEvent* event)
{
    L_IMPL(BlueprintLoader);
    QGraphicsView::mouseReleaseEvent(event);

    if (event->button() == Qt::RightButton)
    {
        l->mpCreatorComboBox->move(event->screenPos().x(), event->screenPos().y());
        l->mpCreatorComboBox->show();
        l->mpCreatorComboBox->setFocus(Qt::FocusReason::MouseFocusReason);
    }
    viewport()->unsetCursor();
}

void BlueprintLoader::keyPressEvent(QKeyEvent* event)
{
    QGraphicsView::keyPressEvent(event);
    if (event->isAccepted())
        return;

    if (event->key() == Qt::Key_Delete) {
        mpImpl->deleteSelectedBKObject();
    }
}

bool BlueprintLoader::event(QEvent* event)
{
    if (event->type() == AnchorMouseEvent) {
        return mpImpl->PreConnectLineEvent(dynamic_cast<PreConnLineEvent*>(event));
    }
        
    return QGraphicsView::event(event);
}