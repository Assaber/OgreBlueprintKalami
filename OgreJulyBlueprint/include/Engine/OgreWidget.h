#pragma once
#include <QWidget>
#include "OgreTimer.h"
#include "OgreRoot.h"
#include "OgreCameraMan.h"

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
    /*两帧之间的间隔，-1代表不受限*/
    int32_t mnFrameInterval = 1000.0f / 60;
    /*Ogre内部的时钟*/
    Ogre::Timer mTimer;
    //Ogre根节点
    Ogre::Root* mpRoot = nullptr;
    //渲染窗口
    Ogre::Window* mpRenderWindow = nullptr;
    //默认场景管理器
    Ogre::SceneManager* mpDefaultSceneManager = nullptr;
    //默认相机
    Ogre::Camera* mpDefaultCamera = nullptr;
    //默认相机的控制句柄
    CameraMan* mpCameraHandle = nullptr;
    //合成器
    Ogre::CompositorWorkspace* mpCompositor = nullptr;
    // 日志
    Ogre::Log* mpLogHandle = nullptr;
    // 资源路径
    std::set<std::string> mExtraRecPath;

public:
    static constexpr const char* strSceneMgrName = "DefaultSceneManager";
};