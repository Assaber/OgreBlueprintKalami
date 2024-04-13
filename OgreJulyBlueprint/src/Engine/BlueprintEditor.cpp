#include "BlueprintEditor.h"

#include <QKeyEvent>
#include <QFileDialog>

#include "TestCard.h"
#include "PrintCard.h"
#include "RenderItemCard.h"
#include "Card/Hlms/PbsDatablock.h"

BlueprintEditor::BlueprintEditor(QWidget* parent/* = nullptr*/)
    : super(parent)
{
}

Ogre::ColourValue BlueprintEditor::toColor(const QColor& color)
{
    return Ogre::ColourValue( 1.0f * color.red() / 255, 1.0f * color.green() / 255, 1.0f * color.blue() / 255, 1.0f * color.alpha() / 255 );
}

Ogre::Vector3 BlueprintEditor::toVec3f(const QColor& color)
{
    return Ogre::Vector3(1.0f * color.red() / 255, 1.0f * color.green() / 255, 1.0f * color.blue() / 255);
}

void BlueprintEditor::keyPressEvent(QKeyEvent* event)
{
    super::keyPressEvent(event);
    if (!event->isAccepted())
    {
        if (event->key() == Qt::Key_N)
        {
            createUnit<TestCard>();
        }
        else if (event->key() == Qt::Key_P)
        {
            createUnit<PrintCard>();
        }
        else if (event->key() == Qt::Key_I)
        {
            createUnit<PbsDatablockCard>();
            createUnit<RenderItemCard>();
        }
        else if (event->key() == Qt::Key_L && (event->modifiers() & Qt::ControlModifier))
        {
            QString scene = QFileDialog::getOpenFileName();
            loadSceneFromJson(scene);
        }
    }
}
