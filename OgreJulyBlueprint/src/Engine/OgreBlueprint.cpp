#include "OgreBlueprint.h"
#include <QDockWidget>
#include <QCoreApplication>
#include <QMenuBar>

OgreBlueprint::OgreBlueprint(QWidget* parent/* = nullptr*/)
    : super(parent)
{
    mpEditorDock = new QDockWidget(tr("编辑器"), this);
    this->addDockWidget(Qt::LeftDockWidgetArea, mpEditorDock);
    mpEditorDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    mpEditor = new BlueprintEditor(mpEditorDock);
    mpEditorDock->setWidget(mpEditor);
    
    mpEngineBrowser = new OgreWidget(this);
    this->setCentralWidget(mpEngineBrowser);
    
    this->resizeDocks(QList<QDockWidget*>() << mpEditorDock, QList<int>() << 400, Qt::Orientation::Horizontal);

    initWindowMenuBar();
}

void OgreBlueprint::drive()
{
    mpEngineBrowser->updateFrame();
}

void OgreBlueprint::initWindowMenuBar()
{
    QMenuBar* mb = menuBar();
    QMenu* windowMenu = new QMenu(tr("窗口"), mb);
    QAction* editorAction = new QAction(tr("编辑器"), windowMenu);
    editorAction->setCheckable(true);
    windowMenu->addAction(editorAction);

    mb->addMenu(windowMenu);

    QObject::connect(windowMenu, &QMenu::aboutToShow, editorAction, [this, editorAction]() {
        editorAction->setChecked(mpEditorDock->isVisible());
        });

    QObject::connect(editorAction, &QAction::triggered, this, [this, editorAction](bool checked) {
        mpEditorDock->setVisible(checked);
        });
}

void OgreBlueprint::closeEvent(QCloseEvent* event)
{
    qApp->postEvent(qApp, new QEvent(QEvent::Quit));
}
