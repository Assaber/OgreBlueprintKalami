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
    // 事件类型
    EventType type = EventType::Pressed;
    // 绘制起始点
    QPointF begin = {};
    // 绘制终止点
    QPointF end = {};
    // 绘制颜色
    QColor* color = nullptr;
    // 发送者
    BKAnchor* sender = nullptr;
};

class TopmostCardEvent : public QEvent
{
public:
    explicit TopmostCardEvent(QGraphicsItem* object);

public:
    QGraphicsItem* mpCard = nullptr;
};