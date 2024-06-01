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
        Fixed,                  ///< 固定大小
        Adaptive,               ///< 自适应
    };

    /// 数据更新回调函数
    // - @param： param 控件值
    // - @return: bool 若为true则停止传递，否则依照锚点绑定传递到下一个结点
    using DataChangeCallback = std::function<bool(const QVariant& param)>;

public:
    explicit BKUnit();
    virtual ~BKUnit();

public:
    /**
     * @brief:                          获取绑定的卡片
     * @return: BKCard*
     * @remark:                         如果未绑定将返回nullptr
     */
    BKCard* getBindCard() const;

    /**
     * @brief:                          默认的数据更新回调函数  
     * @param: const QVariant & param   控件值
     * @return: bool                    若为true则停止传递，否则依照锚点绑定传递到下一个结点
     * @remark: 
     */
    static bool defaultDataChangeCallback(const QVariant& param);

public:
    virtual operator QJsonValue() const = 0;
    virtual bool loadFromJson(const QJsonValue& val) = 0;

    /**
     * @brief:                          控件当前值
     * @return: QVariant                
     * @remark: 
     */
    virtual QVariant data() = 0;

    /**
     * @brief:                          组元复制
     * @return: BKUnit*                 新对象指针
     * @remark:                         可只深度拷贝部分成员属性，大体上是在可扩展单元中使用
     */
    virtual BKUnit* copy() = 0;

protected:
    /**
     * @brief:                          获取绑定的QGraphics对象
     * @return: QGraphicsItem*          对象指针
     * @remark: 
     */
    virtual QGraphicsItem* getGraphicsItem() = 0;
    /**
     * @brief:                          更新数据
     * @param: const QVariant & data    数据变量
     * @return: void                
     * @remark:                         当变量为空，则视为使用当前值触发后续更新，否则使用传入值进行更新
     */
    virtual void dataChanged(const QVariant& data) = 0;
    /**
     * @brief:                          组元大小变更后的主动回调
     */
    virtual void resized();

    /**
     * @brief:                          复制基础组元信息
     * @param: BKUnit * dst             靶对象
     * @return: void
     * @remark: 
     */
    void _copyBasicAttributeTo(BKUnit* dst);

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
    // 控件使能
    bool mbEnable = true;

public:
    // 最小组元高度
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