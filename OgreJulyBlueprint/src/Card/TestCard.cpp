﻿#include "TestCard.h"
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

TestCard::TestCard()
    : BKCard()
{
    _pack({
        BKCreator::create(BKAnchor::AnchorType::None)
            ->append({ BKCreator::create<BKLabel>()
                            ->setText("嘻嘻嘻嘻嘻嘻嘻嘻")
                            ->setAlignment(Qt::AlignCenter)
            }),
        BKCreator::create()
            ->append({ BKCreator::create<BKLabel>()
                            ->setText("Hello world")
                            ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter)
            }),
        BKCreator::create()
            ->append({ BKCreator::create<BKLabel>()
                            ->setText("看我！")
                            ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter)
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
            ->append({ BKCreator::create<BKPushButton>()
                            ->setText("ooook", true),
                       BKCreator::create<BKSpacer>()
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
            ->append({ BKCreator::create<BKSliderBar>(BKSliderBar::DateType::Double)
                            ->setMinimum(0)
                            ->setMaximum(1.0f)
                            ->setCurrentValue(0.5f)
            }),
        BKCreator::create()
            ->append({ BKCreator::create<BKCheckBox>(),
                       BKCreator::create<BKSpacer>(),
            }),
        BKCreator::create()
            ->append({ BKCreator::create<BKColorSelector>(),
                BKCreator::create<BKSpacer>()}),
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
