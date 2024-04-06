#include "BlueprintEditor.h"

#include <QKeyEvent>
#include <QFileDialog>

#include "TestCard.h"
#include "PrintCard.h"

BlueprintEditor::BlueprintEditor(QWidget* parent/* = nullptr*/)
    : super(parent)
{
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
        else if (event->key() == Qt::Key_L && (event->modifiers() & Qt::ControlModifier))
        {
            QString scene = QFileDialog::getOpenFileName();
            loadSceneFromJson(scene);
        }
    }
}
