#pragma once
#include <QMainWindow>
#include "BlueprintEditor.h"
#include "OgreWidget.h"

class OgreBlueprint : public QMainWindow
{
    using super = QMainWindow;
public:
    OgreBlueprint(QWidget* parent = nullptr);

public:
    void drive();

private:
    void initWindowMenuBar();
    void initFileMenuBar();

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:
    BlueprintEditor* mpEditor = nullptr;
    QDockWidget* mpEditorDock = nullptr;
    QDockWidget* mpEngineDock = nullptr;
    OgreWidget* mpEngineBrowser = nullptr;
};