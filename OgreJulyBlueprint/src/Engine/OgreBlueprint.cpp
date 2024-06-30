#include "OgreBlueprint.h"
#include <QDockWidget>
#include <QCoreApplication>
#include <QMenuBar>

OgreBlueprint::OgreBlueprint(QWidget* parent/* = nullptr*/)
    : super(parent)
{
    mpEditorDock = new QDockWidget(tr("Editor"), this);
    this->addDockWidget(Qt::LeftDockWidgetArea, mpEditorDock);
    mpEditorDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    mpEditor = new BlueprintEditor(mpEditorDock);
    mpEditorDock->setWidget(mpEditor);
    
    mpEngineDock = new QDockWidget(tr("Engine"), this);
    this->addDockWidget(Qt::RightDockWidgetArea, mpEngineDock);
    mpEngineDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    mpEngineBrowser = new OgreWidget(this);
    mpEngineDock->setWidget(mpEngineBrowser);

    QFrame* centerFrame = new QFrame(this);
    centerFrame->setFixedWidth(1);
    this->setCentralWidget(centerFrame);
    
    resizeDocks({ mpEditorDock }, { 1000 }, Qt::Orientation::Horizontal);

    initFileMenuBar();
    initWindowMenuBar();
}

void OgreBlueprint::drive()
{
    mpEngineBrowser->updateFrame();
}

void OgreBlueprint::initWindowMenuBar()
{
    QMenu* windowMenu = new QMenu(tr("Window"));
    auto add_window_action = [windowMenu, this](const QString& text, QDockWidget* bind) {
        QAction* action = new QAction(text, windowMenu);
        action->setCheckable(true);
        windowMenu->addAction(action);

        QObject::connect(windowMenu, &QMenu::aboutToShow, action, [this, action, bind]() {
            action->setChecked(bind->isVisible());
            });

        QObject::connect(action, &QAction::triggered, this, [this, action, bind](bool checked) {
            bind->setVisible(checked);
            });
    };
    
    add_window_action(tr("Editor"), mpEditorDock);
    add_window_action(tr("Engine"), mpEngineDock);

    menuBar()->addMenu(windowMenu);
}

void OgreBlueprint::initFileMenuBar()
{
    QMenu* fileMenu = new QMenu(tr("File"));

    QAction* exportAction = new QAction("Export", fileMenu);
    fileMenu->addAction(exportAction);
    QObject::connect(exportAction, &QAction::triggered, mpEditor, &BlueprintEditor::exportScene);

    QAction* importAction = new QAction("Import", fileMenu);
    fileMenu->addAction(importAction);
    QObject::connect(importAction, &QAction::triggered, mpEditor, &BlueprintEditor::importScene);

    menuBar()->addMenu(fileMenu);
}

void OgreBlueprint::closeEvent(QCloseEvent* event)
{
    qApp->postEvent(qApp, new QEvent(QEvent::Quit));
}
