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
    setTitle("Print");

    _pack({
        BKCreator::create(BKAnchor::AnchorType::Input)
            ->append( BKCreator::create<BKLineEdit>()
                ->setText("Uhhhhhh")
                ->setDataChangeCallback([](BKUnit* unit, const QVariant& data) ->bool {
                        if (!data.isValid()) {
                            dynamic_cast<BKLineEdit*>(unit)->setText("");
                        }
                        
                        return true;
                    })
            )
        });
}
