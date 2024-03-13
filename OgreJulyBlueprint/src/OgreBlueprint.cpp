#include "OgreBlueprint.h"
#include "container/BKCell.h"
#include "unit/BKUnitCreator.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"

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
            ->setDefaultIndex(1);
        combText->append(comb);

        pack({ 
            title1, 
            title2,
            title3,
            combText,
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
    if (event->key() == Qt::Key_N)
    {
        createUnit<CustomCard>();
    }

    BlueprintLoader::keyPressEvent(event);
}
