#include "BlueprintEditor.h"

#include <QKeyEvent>
#include <QFileDialog>


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

void BlueprintEditor::importScene()
{
    QString filename = QFileDialog::getOpenFileName();
    if (!filename.isEmpty()) {
        loadSceneFromJson(filename);
    }
}

void BlueprintEditor::exportScene()
{
    QString filename = QFileDialog::getSaveFileName();
    if (!filename.isEmpty()) {
        exportSceneToJson(filename);
    }
}

void BlueprintEditor::keyPressEvent(QKeyEvent* event)
{
    super::keyPressEvent(event);
    if (!event->isAccepted())
    {
        if (event->key() == Qt::Key_L && (event->modifiers() & Qt::ControlModifier)) {
            importScene();
        }
    }
}
