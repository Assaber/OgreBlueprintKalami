#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <memory>
#include <map>
#include "container/BKCard.h"
#include "global_blueprint_kernal.h"

class BKAnchor;
class BKCard;
class BKCreatorMenu;

class _BlueprintKernalExport BlueprintLoader : public QGraphicsView
{
public:
    BlueprintLoader(QWidget* parent = nullptr);
    ~BlueprintLoader();

private:
    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<StandAloneUnit, T>>>
    T* _createUnit(Args &&...args) {
        T* item = new T(std::forward<Args>(args)...);
        StandAloneUnit* unit = dynamic_cast<StandAloneUnit*>(item);

        mScene.addItem(unit->getBindItem());
        unit->syncSoulImprint();

        mUnitsRecord.insert({ unit->getBindItem(), { unit, T::Factory::_cardName} });

        return item;
    }

public:
    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<StandAloneUnit, T>>>
    void createUnit(Args &&...args) {
        _createUnit<T>(std::forward<Args>(args)...);
    }

    void destroyUnit(StandAloneUnit* unit);
    void exportSceneToJson(const QString& path);
    bool loadSceneFromJson(const QString& path);

    void setDarling(BKCard* card);

protected:
    virtual bool event(QEvent* event) override;
    virtual bool eventFilter(QObject*, QEvent*) override;

protected:
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

public:
    class Impl;
    friend class Impl;
    friend class BKCreator;
    Impl* mpImpl = nullptr;

private:
    struct _UnitItem
    {
        StandAloneUnit* item;
        const char* name;
    };

private:
    // 场景
    QGraphicsScene mScene;
    // 单元集
    std::map<QGraphicsItem*, _UnitItem> mUnitsRecord;
};