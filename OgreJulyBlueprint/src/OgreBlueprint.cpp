#include "OgreBlueprint.h"
#include "container/BKCell.h"
#include "unit/BKUnitCreator.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKPushButton.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSpacer.h"

#include <QGraphicsObject>

#include <QDebug>
#include <QKeyEvent>

class CustomCard : public BKCard
{
public:
    CustomCard() : BKCard()
    {
        BKCell* title1 = new BKCell(BKAnchor::AnchorType::None);
        BKCell* title2 = new BKCell();
        BKCell* title3 = new BKCell();
        BKCell* combText = new BKCell(BKAnchor::AnchorType::Output);
        BKCell* combText2 = new BKCell(BKAnchor::AnchorType::Input);
        BKCell* button = new BKCell();
        BKCell* lineEdit = new BKCell();
        BKCell* sliderBar = new BKCell();
        BKCell* checkbox = new BKCell();

        BKUnitCreator creator;
        auto* label = creator.create<BKLabel>();
        label->setText("嘻嘻嘻嘻嘻嘻嘻嘻")
            ->setAlignment(Qt::AlignCenter);
        title1->append(label);

        auto* labe2 = creator.create<BKLabel>();
        labe2->setText("Hello world")
            ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        title2->append(labe2);

        auto* labe3 = creator.create<BKLabel>();
        labe3->setText("看我！")
            ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        title3->append(labe3);

        auto* comb = creator.create<BKComboBox>();
        comb->setItems(QStringList() << "青丝白发一瞬间" << "年华老去像谁言" << "春风犹有怜花意" << "可否许我再少年")
            ->setCurrentIndex(1, false);
        combText->append(comb);
        
        auto* comb2 = creator.create<BKComboBox>();
        comb2->setItems(QStringList() << "青丝白发一瞬间" << "年华老去像谁言" << "春风犹有怜花意" << "可否许我再少年")
            ->setCurrentIndex(1, false);
        combText2->append(comb2);

        auto* btn = creator.create<BKPushButton>();
        btn->setText("ooook", true);
        button->append(btn);
        button->append(creator.create<BKSpacer>());

        auto* le = creator.create<BKLineEdit>();
        le->setText("Snow");
        lineEdit->append(le);

        auto* sl = creator.create<BKSliderBar>(BKSliderBar::DateType::Int);
        sl->setMinimum(15)
            ->setMaximum(60)
            ->setCurrentValue(30);
        sliderBar->append(sl);

        auto* cb = creator.create<BKCheckBox>();
        auto* sp = creator.create<BKSpacer>();
        checkbox->append(cb);
        checkbox->append(sp);

        pack({
            title1,
            title2,
            title3,
            combText2,
            combText,
            button,
            lineEdit,
            sliderBar,
            checkbox,
            });
    }

    ~CustomCard()
    {
        qDebug() << "card released";
    }
};


OgreBlueprint::OgreBlueprint(QWidget* parent/* = nullptr*/)
    : BlueprintLoader(parent)
{
}

void OgreBlueprint::keyPressEvent(QKeyEvent* event)
{
    BlueprintLoader::keyPressEvent(event);
    if (!event->isAccepted())
    {
        if (event->key() == Qt::Key_N)
        {
            createUnit<CustomCard>();
        }
    }
}
