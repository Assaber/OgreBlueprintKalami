#pragma once
#include "global_blueprint_kernal.h"
#include <QGraphicsObject>
#include <QSize>

class BKUnitCreator;
class BKCard;
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

public:
    explicit BKUnit();
    virtual ~BKUnit();

public:
    BKCard* getBindCard() const;

protected:
    virtual QGraphicsItem* getGraphicsItem() = 0;
    virtual void resized();

protected:
    friend class BKCell;
    // 当前组元大小，原则上步应该由组元确定，应当由cell统一调配
    QSizeF mSize = { 0, 0 };
    // 组元的最小大小限制
    QSizeF mMinSize = { 0, 0 };
    // 组元大小策略，整体调整大小时会根据此策略进行判定
    SizePolicy mSizePolicy = SizePolicy::Adaptive;
    // 绑定的卡片
    BKCard* mpBindCard = nullptr;
};

template<typename T>
class _BlueprintKernalExport BKUnitBasic : public BKUnit
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

    inline T* setSizePolicy(SizePolicy policy) {
        mSizePolicy = policy;
        return static_cast<T>(this);
    }
};