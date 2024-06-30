#pragma once
#include "global_blueprint_kernal.h"
#include <QEvent>
#include <QColor>
#include <QPoint>

class BKAnchor;
class QGraphicsItem;

class PreConnLineEvent : public QEvent
{
public:
    enum class EventType : uint8_t
    {
        Pressed = QEvent::GraphicsSceneMousePress,
        Move = QEvent::GraphicsSceneMouseMove,
        Released = QEvent::GraphicsSceneMouseRelease,
    };

public:
    explicit PreConnLineEvent();

public:
    PreConnLineEvent& setType(EventType type);
    PreConnLineEvent& setColorPtr(QColor* color);
    PreConnLineEvent& setBeginPoint(const QPointF& begin);
    PreConnLineEvent& setEndPoint(const QPointF& end);
    PreConnLineEvent& setSender(BKAnchor* sender);

public:
    EventType type = EventType::Pressed;
    QPointF begin = {};
    QPointF end = {};
    QColor* color = nullptr;
    BKAnchor* sender = nullptr;
};

class TopmostCardEvent : public QEvent
{
public:
    explicit TopmostCardEvent(QGraphicsItem* object);

public:
    QGraphicsItem* mpCard = nullptr;
};