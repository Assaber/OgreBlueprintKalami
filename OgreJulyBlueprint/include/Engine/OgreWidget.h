#pragma once
#include <QWidget>
#include "OgreTimer.h"
#include "OgreRoot.h"
#include "OgreCameraMan.h"
#include <time.h>

class OgreWidget : public QWidget
{
    using super = QWidget;

public:
    OgreWidget(QWidget* parent = nullptr);

public:
    void updateFrame();

private:
    void loadPlugin();
    bool selectRenderSystem();
    void createRenderWindow();
    void createScene();
    void loadResource();
    void setupCompositor();

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual QPaintEngine* paintEngine() const override;

private:
    Ogre::Root* mpRoot = nullptr;
    Ogre::Window* mpRenderWindow = nullptr;
    Ogre::SceneManager* mpDefaultSceneManager = nullptr;
    Ogre::Camera* mpDefaultCamera = nullptr;
    CameraMan* mpCameraHandle = nullptr;
    Ogre::CompositorWorkspace* mpCompositor = nullptr;
    Ogre::Log* mpLogHandle = nullptr;

    std::set<std::string> mExtraRecPath;
    // The interval between two frames
    int mFrameDelta = 1000 / 60;
    clock_t mLastRenderTime = 0;

public:
    static constexpr const char* strSceneMgrName = "DefaultSceneManager";
};