#include "BKEvent.h"


PreConnLineEvent::PreConnLineEvent()
    : QEvent(static_cast<QEvent::Type>(AnchorMouseEvent))
{

}

PreConnLineEvent& PreConnLineEvent::setType(EventType type)
{
    this->type = type;
    return *this;
}

PreConnLineEvent& PreConnLineEvent::setColorPtr(QColor* color)
{
    this->color = color;
    return *this;
}

PreConnLineEvent& PreConnLineEvent::setBeginPoint(const QPointF& begin)
{
    this->begin = begin;
    return *this;
}

PreConnLineEvent& PreConnLineEvent::setEndPoint(const QPointF& end)
{
    this->end = end;
    return *this;
}

PreConnLineEvent& PreConnLineEvent::setSender(BKAnchor* sender)
{
    this->sender = sender;
    return *this;
}


TopmostCardEvent::TopmostCardEvent(QGraphicsItem* object)
    : QEvent(static_cast<QEvent::Type>(UpdateTopmostCardEvent))
    , mpCard(object)
{

}
