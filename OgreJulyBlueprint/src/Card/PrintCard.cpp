#include "PrintCard.h"
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



PrintCard::PrintCard()
    : BKCard()
{
    setTitle("打印");

    _pack({
        BKCreator::create(BKAnchor::AnchorType::Input)
            ->setDataType(BKAnchor::Input, BKAnchor::String)
            ->append({ BKCreator::create<BKLineEdit>()
                            ->setText("嘻嘻嘻嘻嘻嘻嘻嘻")
                ->setDataChangeCallback([](BKUnit* unit, const QVariant& data) ->bool {
                        if (!data.isValid()) {
                            dynamic_cast<BKLineEdit*>(unit)->setText("");
                        }
                        
                        return true;
                    })
            })
        });
}
