#pragma once
#include "global_blueprint_kernal.h"
#include "unit/BKUnit.h"
#include <stdint.h>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "BKCreator.h"
#include <set>

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
class _BlueprintKernalExport BKAnchor : public QGraphicsItem, public BKUnitBasic<BKAnchor>
{
    UNIT_FACTORY_ONLY_NAME("_Anchor")

public:
    enum AnchorType
    {
        None                        = 0x00, ///< 无连接锚点
        Input                       = 0x01, ///< 仅输入锚点（左侧）
        Output                      = 0x02, ///< 仅输出锚点（右侧）
        Both                        = Input | Output,   ///< 两端
        MultiConn                   = 0x04, ///< 多重绑定
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
        VecInteger,                 ///< 整型容器：BKIntegerVector
        VecFloat,                   ///< 浮点型容器：BKFloatVector
        
        Custom = 0x0100,            ///< 自定义部分：这里注册时沿用Qt MetaType的ID，理论上是1024往后，但是感觉预制类型也用不上就缩啦
    };


    virtual bool loadFromJson(const QJsonValue& val) override;
    virtual QVariant data() override;
    virtual operator QJsonValue() const override;

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

    /**
     * @brief:                                  锚点将以传入数据进行传递更新
     */
    virtual void dataChanged(const QVariant& data) override;

    /**
     * @brief:                                  [限输入锚点使用]获取输出锚点全部关联数据
     * @param: std::vector<QVariant> & rec      记录
     * @return: int                             记录的个数
     * @remark: 
     */
    int getBindOutputData(std::vector<QVariant>& rec);

public: //注册
    // 会影响绘制时是否填充
    void appendRegist(BKAnchor* anchor, BKConnectingLine* line = nullptr);
    void removeRegist(BKAnchor* anchor);

    /**
     * @brief:                                  
     * @param: BKUnit * unit
     * @return: void
     * @remark:                                 输入锚点和输出锚点都会注册，输入锚点注册为了整个链路相通；输出锚点注册为了连接时使用原数据更新
     */
    void appendRegist(BKUnit* unit);
    void removeRegist(BKUnit* unit);

    /**
     * @brief:                  重定向绑定到卡片
     * @return: bool            是否重定向成功
     * @remark:                 只限于输出锚点可以调用该方法，调用后注册的Unit会被清空
     */
    bool redirectToCard();

public:
    /**
     * @brief:                              注册锚点可以识别的数据类型
     * @param: uint32_t type                注册类型
     * @param: const QColor & color         注册颜色，不给就随机
     * @return: void
     * @remark:                             将结构体通过Q_DECLARE_METATYPE包裹注册，并通过QMetaTypeId<T>::qt_metatype_id返回的id作为type即可。
     * PS： Custom以前的即便传重复了也视为颜色更新，Custom之后的只允许注册一次！
     */
    static void registDataType(uint32_t type, const QColor& color = QColor(qrand() % 255, qrand() % 255, qrand() % 255));

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
    // 锚点小球半径
    static constexpr float mnAnchorBallRadius = 6;

protected:
    virtual QGraphicsItem* getGraphicsItem() override;
    virtual bool sceneEvent(QEvent* event) override;

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};