#pragma once
#include "global_blueprint_kernal.h"
#include "stdint.h"
#include "unit/BKUnit.h"
#include "container/BKAnchor.h"
#include <type_traits>
#include <vector>
#include <memory>
#include <QSizeF>
#include <QJsonArray>

class _BlueprintKernalExport BKCell
{

public:
    BKCell(BKAnchor::AnchorType type = BKAnchor::AnchorType::Both);
    BKCell(uint32_t l, uint32_t r);
    ~BKCell();

public:
    /**
     * @brief:                                              追加单元对象
     * @param: BKUnit* unit                                 单元对象
     * @return: KCell*                                      自身指针
     * @remark:                                             追加的单元对象会按照追加的次序进行显示，并且会自动注册到锚点
     */
    BKCell* append(std::initializer_list<BKUnit*> units);
    /**
     * @brief:                                              追加单元对象
     * @param: BKUnit * units                               单元对象
     * @param: bool regist                                  是否注册到锚点
     * @return: BKCell*                                     自身指针
     * @remark: 
     */
    BKCell* append(BKUnit* units, bool regist = true);

    /**
     * @brief:                                              设置锚点数据类型
     * @param: BKAnchor::AnchorType anchor                  锚点位置
     * @param: uint32_t data                      锚点数据类型
     * @return: BKCell*                                     自身指针
     * @remark: 
     */
    BKCell* setDataType(BKAnchor::AnchorType anchor, uint32_t data);

    /**
     * @brief:                                              更新实际的大小
     * @param: const QSizeF & aim                           目标大小
     * @return:                                             规划后布局大小
     * @remark:                                            
     */
    void updateActualSize(const QSizeF& aim);

    /**
     * @brief:                                              获取理论的大小
     * @return: QSizeF                                      理论大小
     * @remark: 
     */
    QSizeF getTheorySize();

    /**
     * @brief:                                              是否有此锚点
     */
    bool hasAnchor(BKAnchor* anchor);

    /**
     * @brief:                                              获取锚点对象
     * @param: BKAnchor::AnchorType type                    边位
     * @return: BKAnchor*                                   锚点对象
     * @remark: 
     */
    BKAnchor* getAnchor(BKAnchor::AnchorType type);

    /**
     * @brief:                                              以输出锚点的身份更新数据
     * @param: const QVariant & param                       自定义数据
     * @return: bool                                        是否传递成功
     * @remark: 
     */
    bool valueChanged(const QVariant& param);

public:
    /**
     * @brief:                                              将组成控件导出到Json数组
     * @param: QJsonArray & obj                             Json数组
     * @return: bool                                        是否导出成功
     * @remark: 
     */
    bool exportUnitToJson(QJsonArray& obj);

    bool updateUnitFromJson(const QJsonValue& value);
    /**
     * @brief:                                              获取锚点所在的行数
     * @param: BKAnchor * anchor                            锚点对象
     * @return: int 
     * @remark:                                             存在就返回行数（从0计数），没有的话返回-1 
     */ 
    int getMemberRow(BKAnchor* anchor);


private:
    void dispatchPositionChanged();
    void bindCard(BKCard* card, int row);

private:
    friend class BKCard;
    class Impl;
    Impl* mpImpl = nullptr;
};