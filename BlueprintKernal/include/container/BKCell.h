#pragma once
#include "global_blueprint_kernal.h"
#include "stdint.h"
#include "unit/BKUnit.h"
#include "container/BKAnchor.h"
#include <type_traits>
#include <vector>
#include <memory>
#include <QSizeF>

class _BlueprintKernalExport BKCell
{

public:
    BKCell(BKAnchor::AnchorType type = BKAnchor::AnchorType::Both);
    ~BKCell();

public:
    /**
     * @brief:                                            追加单元对象
     * @param: BKUnit* unit                                单元对象
     * @return: void
     * @remark:                                            追加的单元对象会按照追加的次序进行显示
     */
    void append(BKUnit* unit);

    /**
     * @brief:                                            更新实际的大小
     * @param: const QSizeF & aim                        目标大小
     * @return:                                            规划后布局大小
     * @remark:                                            
     */
    void updateActualSize(const QSizeF& aim);

    /**
     * @brief:                                            获取理论的大小
     * @return: QSizeF                                    理论大小
     * @remark: 
     */
    QSizeF getTheorySize();

    void setAnchorOffset(qreal offset);

    /**
     * @brief:                                            是否有此锚点
     */
    bool hasAnchor(BKAnchor* anchor);

private:
    void dispatchPositionChanged();

private:
    friend class BKCard;
    class Impl;
    Impl* mpImpl = nullptr;
};