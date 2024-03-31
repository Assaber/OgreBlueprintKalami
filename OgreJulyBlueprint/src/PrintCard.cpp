#include "PrintCard.h"
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



PrintCard::PrintCard()
    : BKCard()
{
    setTitle("打印");

    _pack({
        BKCreator::create(BKAnchor::AnchorType::Input)
            ->append({ BKCreator::create<BKLineEdit>()
                            ->setText("嘻嘻嘻嘻嘻嘻嘻嘻")
            })
        });
}
