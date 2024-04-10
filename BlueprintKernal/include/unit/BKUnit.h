#pragma once
#include "global_blueprint_kernal.h"
#include <functional>
#include <QGraphicsObject>
#include <QSize>
#include <QJsonValue>

class BKUnitCreator;
class BKCard;
class BKAnchor;
// 组元
class _BlueprintKernalExport BKUnit
{
public:
    /// 大小策略
    enum class SizePolicy : uint8_t
    {
        Fixed,            ///< 固定大小
        Adaptive,        ///< 自适应
    };

    using DataChangeCallback = std::function<bool(const QVariant& param)>;

public:
    explicit BKUnit();
    virtual ~BKUnit();

public:
    BKCard* getBindCard() const;
    static bool defaultDataChangeCallback(const QVariant& param);

public:
    /**
     * @brief:                          获取当前值
     * @return: QJsonValue      
     * @remark:                         导出Json时使用
     */
    virtual QJsonValue getValue() = 0;

    /**
     * @brief:                          获取当前值
     * @param: const QJsonValue & val   
     * @return: bool                    是否获取成功
     * @remark:                         导入Json时使用
     */
    virtual bool setValue(const QJsonValue& val) = 0;

    virtual void registOutputAnchor(BKAnchor* anchor);

protected:
    virtual QGraphicsItem* getGraphicsItem() = 0;
    /**
     * @brief:                          更新数据
     * @param: const QVariant & data    数据变量
     * @return: void                
     * @remark:                         当变量为空，则视为使用当前值触发后续更新，否则使用传入值进行更新
     */
    virtual void dataChanged(const QVariant& data) = 0;
    virtual void resized();

protected:
    friend class BKCell;
    friend class BKAnchor;
    friend class BKConnectingLine;
    // 当前组元大小，原则上步应该由组元确定，应当由cell统一调配
    QSizeF mSize = { 0, 0 };
    // 组元的最小大小限制
    QSizeF mMinSize = { 0, 0 };
    // 组元大小策略，整体调整大小时会根据此策略进行判定
    SizePolicy mSizePolicy = SizePolicy::Adaptive;
    // 绑定的卡片
    BKCard* mpBindCard = nullptr;
    // 绑定的下游锚点
    BKAnchor* mpRightAnchor = nullptr;
    // 数据更新回调
    DataChangeCallback mCallbackFunc;
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

    // 返回true，则停止传递；返回false则根据右锚点继续传递
    inline T* setDataChangeCallback(DataChangeCallback function) {
        mCallbackFunc = function;
        return static_cast<T*>(this);
    }
};