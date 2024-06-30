#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include "BKCreator.hpp"
#include <stdint.h>
#include <set>

/*!
 * \class BKAnchor
 *
 * \brief Anchor on the card, as an endpoint of a cell, the necessary part to connect the two cards
 * \detail 
 * Like the style of listener: 
 *                    Input anchor
 *               ┌───→·←───┐
 *               丨                  丨    
 *               丨                  ↓
 *         Unit  ·                  · Output anchor
 *               ↑                  ↑
 *               └─────────┘
 * 
 * Regist:
 * - Unit will regist to input anchor at the moment of calling BKCell::append, both of input anchor and output anchor will regist current
 *   unit if the enable is true. At the same time, the unit will bind an output anchor and know who to tell if its data changes.
 * - Regarding the relationship between the input and output anchors, they are registered with each other at the time of connection line 
 *   creation(BKConnectingLine::Impl::anchorControl). One of the reasons they register each other is that they can know each other when 
 *   the connection line is removed
 * 
 * Running:
 *  When the data of a unit is actively changed, the output anchor which is registed when unit is constructed will be notify. the output 
 *  anchor will retrieve the currently connected input anchor and transport the variant. The associated input anchor continues to look 
 *  for the bound unit, so that the next unit can passively handle the data update.
 *
 */
class BKUnit;
class BKCell;
class BKConnectingLine;
class _BlueprintKernalExport BKAnchor : public QGraphicsItem, public BKUnitBasic<BKAnchor>
{
    UNIT_FACTORY_ONLY_NAME("_Anchor")

public:
    enum AnchorType
    {
        None                        = 0x00,             ///< Have no anchor
        Input                       = 0x01,             ///< Input anchor(On the left side of the card)
        Output                      = 0x02,             ///< Output anchor(On the right side of the card)
        Both                        = Input | Output,   ///< IO anchor
        MultiConn                   = 0x04,             ///< The identification of supporting multi-input
    };

    enum DataType
    {
        Default = 0x00,             ///< Default(No type distinction is performed)
        Boolean,                    ///< 
        Short,                      ///< 
        Integer,                    ///< 
        String,                     ///< 
        Float,                      ///< 
        Double,                     ///< 
        VecInteger,                 ///< Integral vector：BKIntegerVector
        VecFloat,                   ///< Float vector：BKFloatVector
        
        Custom = 0x0100,            ///< Custom data type:  The Qt MetaType ID is used for registration, which is theoretically 1024 or later. but now it is shrunk
    };

public:
    using super = BKUnitBasic<BKAnchor>;
    BKAnchor(uint32_t type, BKCell* cell);
    ~BKAnchor();

public:
    BKAnchor* setDateType(uint32_t type);
    BKAnchor* setMinSize(const QSizeF& size) = delete;
    BKAnchor* setSizePolicy(SizePolicy policy) = delete;
    virtual BKUnit* copy() override;

public:
    uint32_t getAnchorType();

    uint32_t getDataType();

    bool hasConnected();

    bool inSameCell(BKAnchor* anchor);

    bool hasRegisted(BKAnchor* other);

    /**
     * @remark: Notify the bound unit to pass its own data downward
     */
    virtual void dataChanged() override;

    /**
     * @remark: Notify the bound unit to pass given data downward
     */
    virtual void dataChanged(const QVariant& data) override;

    /**
     * @remark: Only the input anchor can call this method, it will get the association data of all the associated output anchors, 
     * and currently the output anchor only supports the type bound to the card
     */
    int getBindOutputData(std::vector<QVariant>& rec);

public: //Regist
    void appendRegist(BKAnchor* anchor, BKConnectingLine* line = nullptr);
    void removeRegist(BKAnchor* anchor);

    void appendRegist(BKUnit* unit);
    void removeRegist(BKUnit* unit);

    /**
     * @remark: This method can be called only by the output anchor, and the registered unit will be cleared after the call
     */
    bool redirectToCard();

    /**
     * @remark: Register the custom struct via Q_DECLARE_METATYPE, and get the id by QMetaTypeId<T>::qt_metatype_id, After that, take it as parameter 1.
     * In addition to this, for data types before Custom, repeated passes will be treated as updating the anchor color; For data types after Custom, allow one registration!
     */
    static void registDataType(uint32_t type, const QColor& color = QColor(qrand() % 255, qrand() % 255, qrand() % 255));

    static QColor getColorByDataType(uint32_t type);

private:
    void dispatchCellPositionChanged();

    std::vector<BKAnchor*> getRegistAnchors();

    std::set<BKUnit*> getRegistUnits();

    BKCell* getCell();

private:
    friend class BKCell;
    friend class BlueprintLoader;
    friend class BKConnectingLine;
    class Impl;
    Impl* mpImpl;

private:
    static constexpr float mnAnchorBallRadius = 6;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual bool sceneEvent(QEvent* event) override;

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;
};