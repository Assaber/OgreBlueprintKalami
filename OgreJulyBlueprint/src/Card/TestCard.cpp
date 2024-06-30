#include "TestCard.h"
#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "BKCreator.hpp"
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
                ->setText("Uhhhhhhhhhh")
                ->setAlignment(Qt::AlignCenter),
            BKCreator::create<BKColorSelector>()
            }),
        BKCreator::create(BKAnchor::AnchorType::Input, BKCell::Type::ListGroup)
            ->setTemplate({ 
                BKCreator::create<BKLabel>()
                    ->setText("Hello world")
                    ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter),
                BKCreator::create<BKComboBox>()
                    ->setMinWidth(70)
                    ->setItems(QStringList() << "Line 1" << "Line 2" << "Line 3")
                    ->setCurrentIndex(1, false)
            })
            ->setMemberCountChangedCallback([](size_t count, const QVariantList& vl) {
                    qDebug() << "size:" << count;
                })
            ->setMemberDataChangedCallback([](size_t count, const QVariantList& vl) {
                    count = 0;
                    for (QVariantList::const_iterator itor = vl.begin(); itor != vl.end(); ++itor)
                    {
                        if ((++count % 2) == 0) {
                            qDebug() << itor->toString();
                        }
                    }
                }),
        BKCreator::create()
            ->append({ 
                    BKCreator::create<BKLabel>()
                        ->setText("See me")
                        ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter),
                { BKCreator::create<BKTooltip>()
                        ->setTooltip("Nothing"), true }
            }),
        BKCreator::create(BKAnchor::Input, BKAnchor::Output)
            ->append(BKCreator::create<BKComboBox>()
                            ->setItems(QStringList() << "Line 1" << "Line 2" << "Line 3" << "Line 4")
                            ->setCurrentIndex(1, false)
            ),
        BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None)
            ->append({
                  { BKCreator::create<BKComboBox>()
                            ->setItems(QStringList() << "Line 1" << "Line 2" << "Line 3" << "Line 4")
                            ->setCurrentIndex(1, false)
                    , true
                  }
              }),
        BKCreator::create()
            ->append({ { BKCreator::create<BKPushButton>()
                            ->setText("ooook", true), true },
                       BKCreator::create<BKSpacer>(),
                       BKCreator::create<BKTooltip>()->setTooltip(R"(I am the bone of my sword
Steel is my body,and fire is my blood
I have created over a thousand blades
Unknown to Death
Nor known to Life
Have withstood pain to create many weapons
Yet, those hands will never hold anything
So as I pray, Unlimited Blade Works)")
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
