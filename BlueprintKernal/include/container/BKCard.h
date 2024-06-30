#pragma once
#include "global_blueprint_kernal.h"
#include "container/StandAloneUnit.h"
#include "container/BKAnchor.h"
#include "container/BKCell.h"
#include "BlueprintLoader.h"
#include <QJsonObject>

class _BlueprintKernalExport BKCard : public StandAloneUnit
{
public:
    BKCard();
    virtual ~BKCard();

public:
    void setTitle(const QString& title);
    virtual QGraphicsItem* getBindItem() override;

    /**
     * @remark: The default is valid QVariant, if you want the card to return the value of its own struct, you need to override this virtual function.
     */
    virtual QVariant getCurrentCardValue();

public:
    bool loadFromJson(const QJsonArray& obj, const QPoint& pos = QPoint(0, 0));
    QJsonArray exportToJson();
    /**
     * @remark: Gets the anchor for the specified number and type of rows, Don't pass Both!!
     */
    BKAnchor* getRowAnchor(int row, BKAnchor::AnchorType type);

    /**
     * @remark: An Inner identifier, because the program will release all cards when it exits, and the release of cards will sometimes call back notifications, 
     * so provide a judgment identifier before releasing itself
     */
    bool isStillAlive();

    /**
     * @remark: re-calc self layout
     */
    void rePackage();

protected:
    /**
     * @remark: The custom card needs to add all the cells to itself by this method, and this function will calculate the layout of the card
     */
    void _pack(std::initializer_list<BKCell*> cells);
   
private:
    class Impl;
    Impl* mpImpl = nullptr;
    static const char* mstrCardName;

private:
    friend class BKCreator;
};

