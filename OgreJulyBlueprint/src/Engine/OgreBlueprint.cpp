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
    
    mpEngineDock = new QDockWidget(tr("引擎视口"), this);
    this->addDockWidget(Qt::RightDockWidgetArea, mpEngineDock);
    mpEngineDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    mpEngineBrowser = new OgreWidget(this);
    mpEngineDock->setWidget(mpEngineBrowser);

    QFrame* centerFrame = new QFrame(this);
    centerFrame->setFixedWidth(1);
    this->setCentralWidget(centerFrame);
    
    resizeDocks({ mpEditorDock }, { 1000 }, Qt::Orientation::Horizontal);

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
    auto add_window_action = [mb, windowMenu, this](const QString& text, QDockWidget* bind) {
        QAction* action = new QAction(text, windowMenu);
        action->setCheckable(true);
        windowMenu->addAction(action);

        mb->addMenu(windowMenu);

        QObject::connect(windowMenu, &QMenu::aboutToShow, action, [this, action, bind]() {
            action->setChecked(bind->isVisible());
            });

        QObject::connect(action, &QAction::triggered, this, [this, action, bind](bool checked) {
            bind->setVisible(checked);
            });
    };
    
    add_window_action(tr("编辑器"), mpEditorDock);
    add_window_action(tr("引擎视口"), mpEngineDock);
}

void OgreBlueprint::closeEvent(QCloseEvent* event)
{
    qApp->postEvent(qApp, new QEvent(QEvent::Quit));
}
