#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <stdint.h>
#include <QGraphicsScene>
#include <QGraphicsItem>

/*!
 * \class BKAnchor
 *
 * \brief 卡片（单元）上的锚点，用以相互连接
 * \detail 
 * 整体风格为监听者模式：将unit注册到输入类型的锚点，将输出类型的锚点注册给unit，将输入锚点注册给输出锚点
 *                      输入锚点
 *               ┌───→·────┐
 *               丨                  丨    
 *               丨                  ↓
 *         Unit  ·                  · 输出锚点
 *               ↑                  丨
 *               └─────────┘
 * 运作逻辑：
 *     Unit变更触发绑定的回调函数(valueChanged)，回调会遍历输出锚点列表（其实应该是1个）；如果输出锚点存在输入锚点的绑定，且输入锚点存在Unit的绑定，则继续触发回调(valueChange)
 *     如果一组cell的两个锚点都被连接，则Unit的编辑功能会被关闭。
 *
 * \author Assaber
 * \date 2024.02.25
 */
class BKUnit;
class BKCell;
class BKConnectingLine;
class BKAnchor : public QGraphicsItem, public BKUnitBasic<BKAnchor>
{
public:
    enum AnchorType
    {
        None                        = 0x00, ///< 无连接锚点
        Input                       = 0x01, ///< 仅输入锚点（左侧）
        Output                      = 0x02, ///< 仅输出锚点（右侧）
        Both                        = Input | Output,///< 两端
    };

    enum DataType
    {
        Default = 0x00,             ///< 缺省数据类型（不进行类型区分）
        Boolean,                    ///< bool类型
        Short,                      ///< short类型
        Integer,                    ///< 整数类型
        String,                     ///< 字符串类型
        Float,                      ///< 小数类型
        Double,                     ///< 双精度浮点数

        Custom,
    };

public:
    using super = BKUnitBasic<BKAnchor>;
    BKAnchor(AnchorType type, BKCell* cell);
    ~BKAnchor();

public:
    BKAnchor* setDateType(DataType type);
    BKAnchor* setBezierOffset(int offset);

public:
    AnchorType getAnchorType();
    DataType getDataType();
    bool hasConnected();
    /**
     * @brief:                                  是否在相同的组元中
     * @param: BKAnchor * anchor                给定的另一个锚点
     * @return: bool                            4就4，不4就不4
     * @remark: 
     */
    bool inSameCell(BKAnchor* anchor);

    /**
     * @brief:                                  是否已经注册过了
     * @param: BKAnchor * other                 给定的另一个锚点
     * @return: bool            
     * @remark: 
     */
    bool hasRegisted(BKAnchor* other);

public: //注册
    // 会影响绘制时是否填充
    void appendRegist(BKAnchor* anchor, BKConnectingLine* line = nullptr);
    void removeRegist(BKAnchor* anchor);

    void appendRegist(BKUnit* unit);
    void removeRegist(BKUnit* unit);

public:
    BKAnchor* setMinSize(const QSizeF& size) = delete;
    BKAnchor* setSizePolicy(SizePolicy policy) = delete;

private:
    void dispatchCellPositionChanged();

private:
    friend class BKCell;
    friend class BlueprintLoader;
    friend class BKConnectingLine;
    class Impl;
    Impl* mpImpl;

private:
    // 锚点小球半径
    static constexpr float mnAnchorBallRadius = 6;


protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual bool sceneEvent(QEvent* event) override;

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};