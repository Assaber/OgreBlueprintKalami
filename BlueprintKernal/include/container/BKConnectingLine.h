#pragma once
#include "global_blueprint_kernal.h"
#include "container/StandAloneUnit.h"

class BKAnchor;
class _BlueprintKernalExport BKConnectingLine : public StandAloneUnit
{
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

protected:
	virtual QGraphicsItem* getBindItem() override;

private:
    class Impl;
    Impl* mpImpl = nullptr;
};

