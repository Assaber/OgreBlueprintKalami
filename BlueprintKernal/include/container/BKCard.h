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
    /**
     * @brief:                  获取当前卡片的绑定值
     * @return: QVariant
     * @remark:                 原则上只有当子输出锚点在所在的Cell中无法找到绑定对象时(nullptr)，才会调用该方法获取整个卡片的值
     * 如果有需要请重写该函数，默认为缺省QVariant
     */
    virtual QVariant getCurrentCardValue();

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

