#pragma once
#include "global_blueprint_kernal.h"
#include <functional>
#include <QGraphicsObject>
#include <QSize>
#include <QJsonValue>

class BKUnitCreator;
class BKCard;
class BKAnchor;

class _BlueprintKernalExport BKUnit
{
public:
    enum class SizePolicy : uint8_t
    {
        Fixed,
        Adaptive,
    };

    /// Callback function for data updates
    // - @param： unit  
    //          Which Unit made the update
    // - @param： param
    //          Data
    // - @return: bool
    //          If true, the updated value will stop being passed to the bound output anchor, otherwise it will continue to be passed
    using DataChangeCallback = std::function<bool(BKUnit* unit, const QVariant& param)>;

public:
    explicit BKUnit();
    virtual ~BKUnit();

public:
    /**
     * @remark: If not bound, nullptr will be returned                   
     */
    BKCard* getBindCard() const;

    static bool defaultDataChangeCallback(BKUnit* unlit, const QVariant& param);

public:
    virtual operator QJsonValue() const = 0;
    virtual bool loadFromJson(const QJsonValue& val) = 0;

    virtual QVariant data() = 0;

    /**
     * @remark: You can deep copy only a subset of member attributes, which is largely used in template cell(BKCell::Type::ListGroup)
     */
    virtual BKUnit* copy() = 0;

protected:
    virtual QGraphicsItem* getGraphicsItem() = 0;
    /**         
     * @remark: If the param is QVariant(), the current value is used to trigger subsequent updates, otherwise the param is used
     */
    virtual void dataChanged(const QVariant& data) = 0;
    /**
     * @remark: QVariant::Invalid parameter triggers a callback for data change. In principle, only after the connection line is disconnected, 
     * it is used to notify the next node that the data has changed, and will not change the control value actually. you can set a new value 
     * in the callback by discussing the situation
     */
    virtual void dataChanged();
    /**
     * @brief: The function will be called after the size has been changed, and you can override this function to update the layout inside 
     * some units
     */
    virtual void resized();
    /**
     * @remark: Copy some necessary and basic attribute, you can call this function in copy()
     */
    void _copyBasicAttributeTo(BKUnit* dst);

protected:
    friend class BKCell;
    friend class BKAnchor;
    friend class BKConnectingLine;
    // The size of unit. In principle, it should not be determined by the unit, it should be set by it's cell
    QSizeF mSize = { 0, 0 };
    // Minimum limit on unit size
    QSizeF mMinSize = { 0, 0 };
    // Unit size policy, which is used to determine the layout when calculating the layout
    SizePolicy mSizePolicy = SizePolicy::Adaptive;

    BKCard* mpBindCard = nullptr;
    // The associated output anchor
    BKAnchor* mpRightAnchor = nullptr;

    DataChangeCallback mCallbackFunc;
    // Whether the unit can respond to certain interactions
    bool mbEnable = true;

public:
    static constexpr int minUnitHeight = 20;
};

template<typename T>
class BKUnitBasic : public BKUnit
{
public:
    using super = BKUnit;
    using super::super;

public:
    inline T* setFixedSize(const QSizeF& size) {
        mSize = mMinSize = size;
        mSizePolicy = SizePolicy::Fixed;
        return static_cast<T*>(this);
    }

    inline T* setMinSize(const QSizeF& size) {
        mMinSize = size;
        return static_cast<T*>(this);
    }

    inline T* setMinWidth(qreal width) {
        mMinSize.setWidth(width);
        return static_cast<T*>(this);
    }

    inline T* setSizePolicy(SizePolicy policy) {
        mSizePolicy = policy;
        return static_cast<T*>(this);
    }

    inline T* setDataChangeCallback(DataChangeCallback function) {
        mCallbackFunc = function;
        return static_cast<T*>(this);
    }

    inline T* setEnable(bool enable) {
        mbEnable = enable;
        return static_cast<T*>(this);
    }
};