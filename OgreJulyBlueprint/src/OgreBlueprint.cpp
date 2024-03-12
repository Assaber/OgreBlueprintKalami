#include "OgreBlueprint.h"
#include "container/BKCell.h"
#include "unit/BKUnitCreator.h"
#include "unit/BKLabel.h"

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

        pack({ 
            title1, 
            title2,
            title3,
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
