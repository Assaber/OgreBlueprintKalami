#include "OgreBlueprint.h"
#include "TestCard.h"
#include "PrintCard.h"
#include <QKeyEvent>


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
            createUnit<TestCard>();
        }
        else if(event->key() == Qt::Key_P)
        {
            createUnit<PrintCard>();
        }
    }
}
