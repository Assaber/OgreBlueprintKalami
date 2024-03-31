#pragma once
#include "global_blueprint_kernal.h"
#include "container/StandAloneUnit.h"
#include <QJsonObject>

class BKAnchor;
class BKCard;
class _BlueprintKernalExport BKConnectingLine : public StandAloneUnit
{
public:
    struct BasicInfo
    {
        BKCard* rCard = nullptr;
        int rIndex = -1;
        BKCard* lCard = nullptr;
        int lIndex = -1;
    };

public:
    BKConnectingLine();
    BKConnectingLine(const QColor& color, BKAnchor* a1, BKAnchor* a2);
    virtual ~BKConnectingLine();

public:
    BKConnectingLine& setColor(const QColor& color);
    BKConnectingLine& setVisible(bool visible);
    BKConnectingLine& setPainterPath(const QPainterPath& path);
    BKConnectingLine& setPainterPath(const QPointF& begin);
    BKConnectingLine& setPainterPath(const QPointF& begin, const QPointF& end);
    void update();
    void updateByBind();

public:
    static QPainterPath createPainterPath(const QPointF* begin, const QPointF* end = nullptr);
    bool getBasicInfo(BasicInfo& info);

protected:
	virtual QGraphicsItem* getBindItem() override;

private:
    friend class BlueprintLoader;
    friend class BKCreator;
    class Factory {
    public:
        static constexpr char* _cardName = "_ConnectLine";
    };

private:
    class Impl;
    Impl* mpImpl = nullptr;
};

