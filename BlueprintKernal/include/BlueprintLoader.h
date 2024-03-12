#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <memory>
#include <map>
#include "container/BKCard.h"
#include "global_blueprint_kernal.h"

class BKAnchor;
class _BlueprintKernalExport PreConnLineEvent : public QEvent
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

class BKCard;
class _BlueprintKernalExport BlueprintLoader : public QGraphicsView
{
public:
    BlueprintLoader(QWidget* parent = nullptr);
    ~BlueprintLoader();

public:
    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<StandAloneUnit, T>>>
    void createUnit(Args &&...args) {
        T* item = new T(std::forward<Args>(args)...);
        StandAloneUnit* unit = dynamic_cast<StandAloneUnit*>(item);

        mScene.addItem(unit->getBindItem());
        unit->syncSoulImprint();

        mUnitsRecord.insert({ unit->getBindItem(), unit });
    }

    void destroyUnit(StandAloneUnit* unit);

protected:
    virtual bool event(QEvent* event) override;

protected:
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

public:
    class Impl;
    friend class Impl;
    Impl* mpImpl = nullptr;

private:
    // 场景
    QGraphicsScene mScene;
    // 单元集
    std::map<QGraphicsItem*, StandAloneUnit*> mUnitsRecord;
};