#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include "container/BKAnchor.h"
#include <type_traits>
#include <vector>
#include <memory>
#include <stdint.h>

#include <QJsonArray>
#include <QSizeF>


class _BlueprintKernalExport BKCell
{
public:
    enum class Type : uint8_t
    {
        SingleLine,         ///< Normal mode, the unit in a cell is determinist
        ListGroup,          ///< Special mode, this cell is extensible, it will provide buttons to add and remove, and of course you need to provide a template for the extension row
    };

    struct RegistableUnitPair
    {
        BKUnit* unit;
        bool regist;

        RegistableUnitPair() = delete;
        RegistableUnitPair(BKUnit* u, bool r = false) {
            unit = u;
            regist = r;
        }
    };

public:
    BKCell(BKAnchor::AnchorType anchor = BKAnchor::AnchorType::Both, Type type = Type::SingleLine);
    BKCell(uint32_t l, uint32_t r, Type type = Type::SingleLine);
    ~BKCell();

public:                 // Type::SingleLine
    /**
     * @remark: Add summoned units in batches, Note that RegistableUnitPair defaults to unregistered. in other words, it will not be registered to the input and output anchors 
     */
    BKCell* append(std::initializer_list<RegistableUnitPair> units);
    /**
     * @remark: append item one by one
     */
    BKCell* append(BKUnit* units, bool regist = true);


public:                // Type::ListGroup
    using GroupMemberChangedFunc = std::function<void(size_t, const QVariantList&)>;        // size_t: total count; const QVariantList& : data for all members

    BKCell* setTemplate(std::initializer_list<BKUnit*> units);
    /**
     * @remark: Set the upper limit on the number of new members. If not, the number of new members will only be subject to the data type
     */
    BKCell* setMemberMaximum(int count);
    BKCell* setMemberDataChangedCallback(GroupMemberChangedFunc function);
    BKCell* setMemberCountChangedCallback(GroupMemberChangedFunc function);

    // operate for template
    bool push(uint32_t count = 1);
    bool pop();

public:
    /**
     * @remark: For invisible components, the use of connection lines will not be provided; If the enable changes to false, the connect line will be released ? 
     * The conceptual interface has not yet been implemented
     */
    void setVisible(bool visible) = delete;

public:
    /**
     * @remark: param 2 is Anchor::DataType or custom type id. see Anchor::registDataType for detail
     */
    BKCell* setDataType(BKAnchor::AnchorType anchor, uint32_t data);

    void updateActualSize(const QSizeF& aim);

    QSizeF getTheorySize();

    bool hasAnchor(BKAnchor* anchor);

    BKAnchor* getAnchor(BKAnchor::AnchorType type);

    /**
     * @remark: Let the internal output anchor pass down with a given QVariant
     */
    bool valueChanged(const QVariant& param);

public:
    bool exportUnitToJson(QJsonArray& obj);

    bool updateUnitFromJson(const QJsonValue& value);
    /**
     * @remark:                                             Gets the number of rows where the anchor is located, if it doesn't exist, it returns -1
     */ 
    int getMemberRow(BKAnchor* anchor);

private:
    void dispatchPositionChanged();

    void bindCard(BKCard* card, int row);

    QGraphicsItem* getGraphicsItem();

private:
    friend class BKCard;
    class Impl;
    Impl* mpImpl = nullptr;
};