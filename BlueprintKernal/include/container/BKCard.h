#pragma once
#include "global_blueprint_kernal.h"
#include "container/StandAloneUnit.h"
#include "container/BKCell.h"
#include "container/BKAnchor.h"
#include "BKCreator.h"
#include "BlueprintLoader.h"
#include <QJsonObject>

class _BlueprintKernalExport BKCard : public StandAloneUnit
{
    CREATE_CARD_FACTORY(BKCard)
public:
    BKCard();
    virtual ~BKCard();

public:
    void setTitle(const QString& title);
    virtual QGraphicsItem* getBindItem() override;

public:
    bool loadFromJson(const QJsonArray& obj, const QPoint& pos = QPoint(0, 0));
    QJsonArray exportToJson();
    BKAnchor* getRowAnchor(int row, BKAnchor::AnchorType type);

protected:
    void _pack(std::initializer_list<BKCell*> cells);
   
private:
    class Impl;
    Impl* mpImpl = nullptr;
    static const char* mstrCardName;

private:
    friend class BKCreator;
};

