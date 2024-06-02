#include "TestCard.h"
#include "BKCreator.h"
#include "container/BKCell.h"
#include "BKCreator.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKPushButton.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKColorSelector.h"
#include "unit/BKColorSelectorEx.h"
#include "unit/BKPixmap.h"
#include "unit/BKVectorEditor.h"
#include "unit/BKTooltip.h"

#include <QDebug>

TestCard::TestCard()
    : BKCard()
{
    _pack({
        BKCreator::create(BKAnchor::AnchorType::None)
            ->append({
            BKCreator::create<BKLabel>()
                ->setText("嘻嘻嘻嘻嘻嘻嘻嘻")
                ->setAlignment(Qt::AlignCenter),
            BKCreator::create<BKColorSelector>()
            }),
        BKCreator::create(BKAnchor::AnchorType::Input, BKCell::Type::ListGroup)
            ->setTemplate({ 
                BKCreator::create<BKLabel>()
                    ->setText("Hello world")
                    ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter),
                BKCreator::create<BKComboBox>()
                    ->setItems(QStringList() << "焚我残躯" << "熊熊圣火" << "生亦何欢" << "死亦何苦" << "为善除恶" << "为光明故" << "喜乐悲愁" << "皆归尘土")
                    ->setCurrentIndex(1, false)
            })
            ->setMemberCountChangedCallback([](size_t count, const QVariantList& vl) {
                    qDebug() << "size:" << count;
                })
            ->setMemberDataChangedCallback([](size_t count, const QVariantList& vl) {
                    count = 0;      // 兄弟，你好香
                    for (QVariantList::const_iterator itor = vl.begin(); itor != vl.end(); ++itor)
                    {
                        if ((++count % 2) == 0)
                        {
                            qDebug() << itor->toString();
                        }
                    }
                }),
        BKCreator::create()
            ->append({ 
                    BKCreator::create<BKLabel>()
                        ->setText("看我！")
                        ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter),
                { BKCreator::create<BKTooltip>()
                        ->setTooltip("你瞅啥"), true }
            }),
        BKCreator::create(BKAnchor::Input, BKAnchor::Output)
            ->append({ BKCreator::create<BKComboBox>()
                            ->setItems(QStringList() << "青丝白发一瞬间" << "年华老去像谁言" << "春风犹有怜花意" << "可否许我再少年")
                            ->setCurrentIndex(1, false)
            }),
        BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None)
            ->append({ BKCreator::create<BKComboBox>()
                            ->setItems(QStringList() << "青丝白发一瞬间" << "年华老去像谁言" << "春风犹有怜花意" << "可否许我再少年")
                            ->setCurrentIndex(1, false)
            }),
        BKCreator::create()
            ->append({ { BKCreator::create<BKPushButton>()
                            ->setText("ooook", true), true },
                       BKCreator::create<BKSpacer>(),
                       BKCreator::create<BKTooltip>()->setTooltip(R"(噫吁嚱，危乎高哉！
蜀道之难，难于上青天！
蚕丛及鱼凫，开国何茫然。
尔来四万八千岁，不与秦塞通人烟。
西当太白有鸟道，可以横绝峨眉巅。
地崩山摧壮士死，然后天梯石栈相钩连。
上有六龙回日之高标，下有冲波逆折之回川。
黄鹤之飞尚不得过，猿猱欲度愁攀援。
青泥何盘盘，百步九折萦岩峦。
扪参历井仰胁息，以手抚膺坐长叹。
问君西游何时还？畏途巉岩不可攀。
但见悲鸟号古木，雄飞雌从绕林间。
又闻子规啼夜月，愁空山。
蜀道之难，难于上青天，使人听此凋朱颜。
连峰去天不盈尺，枯松倒挂倚绝壁。
飞湍瀑流争喧豗，砯崖转石万壑雷。
其险也如此，嗟尔远道之人胡为乎来哉！
剑阁峥嵘而崔嵬，一夫当关，万夫莫开。
所守或匪亲，化为狼与豺。
朝避猛虎，夕避长蛇；磨牙吮血，杀人如麻。
锦城虽云乐，不如早还家。
蜀道之难，难于上青天，侧身西望长咨嗟)")
            }),
        BKCreator::create()
            ->append({ BKCreator::create<BKLineEdit>()
                            ->setText("Snow")
            }),
        BKCreator::create()
            ->append({ BKCreator::create<BKSliderBar>()
                            ->setMinimum(15)
                            ->setMaximum(60)
                            ->setCurrentValue(30)
            }),
        BKCreator::create()
            ->append({ BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)
                            ->setMinimum(0)
                            ->setMaximum(1.0f)
                            ->setCurrentValue(0.5f)
            }),
        BKCreator::create()
            ->append({ 
                { BKCreator::create<BKCheckBox>(), true },
                BKCreator::create<BKSpacer>(),
            }),
        BKCreator::create()
            ->append({ 
                { BKCreator::create<BKColorSelector>(), true },
                BKCreator::create<BKSpacer>()
                }),
        BKCreator::create()
            ->append({ BKCreator::create<BKColorSelectorEx>() }),
        BKCreator::create()
            ->append({ BKCreator::create<BKPixmap>() }),
         BKCreator::create()
            ->setDataType(BKAnchor::Input, BKAnchor::VecInteger)
            ->append( BKCreator::create<BKVectorEditor>()
            ->setItemInLine(1)
            ->setNames({"r", "g"})
            ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{ 1, 2, 3 }))),
        });
}
