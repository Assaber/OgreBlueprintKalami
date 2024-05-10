#include "OgreWidget.h"
#include "OgreLog.h"
#include "OgreRoot.h"
#include "OgreLogManager.h"
#include "OgreWindow.h"
#include "OgreArchiveManager.h"
#include "Compositor/OgreCompositorManager2.h"
#include "OgreHlms.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"
#include "OgreItem.h"

#include <vector>
#include <map>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

class CustomLogListener : public Ogre::LogListener
{
public:
    virtual void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName, bool& skipThisMessage) override
    {
        switch (lml)
        {
        case Ogre::LogMessageLevel::LML_TRIVIAL:
            qDebug() << message.c_str();
            break;
        case Ogre::LogMessageLevel::LML_NORMAL:
            qInfo() << message.c_str();
            break;
        case Ogre::LogMessageLevel::LML_CRITICAL:
            qCritical() << message.c_str();
            break;
        }
    }
};

OgreWidget::OgreWidget(QWidget* parent/* = nullptr*/)
    : super(parent)
{
    setAttribute(Qt::WA_PaintOnScreen, true);

    mpRoot = new Ogre::Root(nullptr, "", "", "");           // 在4.0.0版本中，参数1变为了abiCookie检测 (好像3.0就改了
    Ogre::LogManager& logMgr = Ogre::LogManager::getSingleton();
    mpLogHandle = logMgr.getDefaultLog();
    mpLogHandle->setDebugOutputEnabled(false);
    mpLogHandle->addListener(new CustomLogListener());

    loadPlugin();

    if (!selectRenderSystem())
        return;

    createRenderWindow();

    createScene();

    loadResource();

    setupCompositor();
}

void OgreWidget::updateFrame()
{
    static clock_t  currentClock;
    currentClock = clock();

    if (currentClock - mLastRenderTime < mFrameDelta)
        return;

    mLastRenderTime = currentClock;

    try
    {
        mpRoot->renderOneFrame();
    }
    catch (const Ogre::Exception& e)
    {
        mpLogHandle->logMessage(e.what(), Ogre::LogMessageLevel::LML_CRITICAL);
    }
    catch (const std::exception& e)
    {
        mpLogHandle->logMessage(e.what(), Ogre::LogMessageLevel::LML_CRITICAL);
    }
    catch (...)
    {
        mpLogHandle->logMessage("未知的渲染错误", Ogre::LogMessageLevel::LML_CRITICAL);
    }
}

void OgreWidget::loadPlugin()
{
    QDir currentDir(QCoreApplication::applicationDirPath());
    QStringList filter;

    static_assert(OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WINRT, "臣妾不知道其他平台怎么判定呀orz");
    filter << "Plugin_*.dll"
        << "RenderSystem_*.dll";

    QFileInfoList files = currentDir.entryInfoList(filter, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    for (const QFileInfo& info : files)
    {
        std::string pluginName = info.fileName().toStdString();
        mpRoot->loadPlugin(pluginName, false, nullptr);
        mpLogHandle->logMessage("加载插件:" + pluginName);
    }
}

bool OgreWidget::selectRenderSystem()
{
    // todo...
    // 添加一个窗口来选择阵营


    static constexpr char* rsList[] = {
        "OpenGL 3+ Rendering Subsystem",
        "Direct3D11 Rendering Subsystem",
        "Vulkan Rendering Subsystem",
        "NULL Rendering Subsystem",
    };
    const char* rs = rsList[0];

    Ogre::RenderSystemList rList = mpRoot->getAvailableRenderers();
    Ogre::RenderSystemList::iterator itor = rList.begin();
    while (itor != rList.end()) {
        Ogre::RenderSystem* rSys = *(itor);
        Ogre::String name = rSys->getName();
        if (rSys->getName().find(rs) != std::string::npos)
        {
            mpRoot->setRenderSystem(rSys);
            mpLogHandle->logMessage("渲染系统加载完成: " + std::string(rs));
            break;
        }

        ++itor;
    }

    if (itor == rList.end())
    {
        mpLogHandle->logMessage("渲染系统加载失败: " + std::string(rs), Ogre::LogMessageLevel::LML_CRITICAL);
        return false;
    }

    return true;
}

void OgreWidget::createRenderWindow()
{
    mpRoot->initialise(false);
    Ogre::NameValuePairList params;
    params["left"] = "0";
    params["top"] = "0";
    params["border"] = "none";
    params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)winId());
    mpRenderWindow = mpRoot->createRenderWindow("Tiny Seed", width(), height(), false, &params);
    mpRenderWindow->_setVisible(true);
}

void OgreWidget::createScene()
{
    mpDefaultSceneManager = mpRoot->createSceneManager(Ogre::ST_GENERIC, 1u, strSceneMgrName);
    Ogre::SceneNode* rootSceneNode = mpDefaultSceneManager->getRootSceneNode();

    mpDefaultCamera = mpDefaultSceneManager->createCamera("DefaultCamera");
    mpDefaultCamera->setNearClipDistance(1e-5);
    Ogre::SceneNode* camNode = rootSceneNode->createChildSceneNode();
    mpDefaultCamera->detachFromParent();
    camNode->attachObject(mpDefaultCamera);
    mpCameraHandle = new CameraMan(camNode);
    mpCameraHandle->setTarget(mpDefaultSceneManager->getRootSceneNode());
    mpRoot->addFrameListener(mpCameraHandle);

    mpDefaultSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f),
        Ogre::ColourValue(1.0f, 1.0f, 1.0f),
        Ogre::Vector3(-1, -1, -1));

    Ogre::SceneNode* spotlightNode = rootSceneNode->createChildSceneNode();
    spotlightNode->setPosition(0, 2, 0);
    Ogre::Light* l = mpDefaultSceneManager->createLight();
    spotlightNode->attachObject(l);
    l->setType(Ogre::Light::LT_SPOTLIGHT);
    l->setDirection({ 0, 0, 0 });
    l->setSpotlightFalloff(0.6f);
}

void OgreWidget::loadResource()
{
    auto rm = Ogre::ResourceGroupManager::getSingletonPtr();
    auto archiveManager = Ogre::ArchiveManager::getSingletonPtr();

    static const std::map<Ogre::String, std::vector<const char*>> resource = {
        {
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, {              // 资源都会拷贝到输出目录的上一层
                "../../media/Common",
                "../../media/Common/Any",
                "../../media/Common/GLSL",
                "../../media/Common/HLSL",
                "../../media/Common/Metal",
                "../../media/Hlms/Common/Any",
                "../../media/Hlms/Common/GLSL",
                "../../media/Hlms/Common/HLSL",
                "../../media/Hlms/Common/Metal",
                "../../media/models",
                "../../media/material",
            }
        }
    };

    for (const auto& group : resource)
        for (const auto& item : group.second)
            rm->addResourceLocation(item, "FileSystem", group.first);

    // HLMS
    {
        Ogre::String hlms;
        Ogre::StringVector hlmsItems;

        auto regist_hlms = [&](const char* prefix) -> std::tuple<Ogre::Archive*, Ogre::ArchiveVec> {
            Ogre::Archive* ret = archiveManager->load(prefix + hlms, "FileSystem", true);
            Ogre::ArchiveVec retVec;
            for (const auto& item : hlmsItems)
            {
                auto library = archiveManager->load(prefix + item, "FileSystem", true);
                retVec.push_back(library);
            }
            return { ret, retVec };
        };


        Ogre::HlmsUnlit::getDefaultPaths(hlms, hlmsItems);
        auto unlitResult = regist_hlms("../../media/");
        auto hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(std::get<0>(unlitResult), &std::get<1>(unlitResult));
        mpRoot->getHlmsManager()->registerHlms(hlmsUnlit);

        Ogre::HlmsPbs::getDefaultPaths(hlms, hlmsItems);
        auto pbsResult = regist_hlms("../../media/");
        auto hlmsPbs = OGRE_NEW Ogre::HlmsPbs(std::get<0>(pbsResult), &std::get<1>(pbsResult));
        mpRoot->getHlmsManager()->registerHlms(hlmsPbs);

        if (mpRoot->getRenderSystem()->getName().find("Direct3D11") != std::string::npos)
        {
            // Set lower limits 512kb instead of the default 4MB per Hlms in D3D 11.0
            // and below to avoid saturating AMD's discard limit (8MB) or
            // saturate the PCIE bus in some low end machines.
            bool supportsNoOverwriteOnTextureBuffers;
            mpRoot->getRenderSystem()->getCustomAttribute("MapNoOverwriteOnDynamicBufferSRV",
                &supportsNoOverwriteOnTextureBuffers);

            if (!supportsNoOverwriteOnTextureBuffers)
            {
                hlmsPbs->setTextureBufferDefaultSize(512 * 1024);
                hlmsUnlit->setTextureBufferDefaultSize(512 * 1024);
            }
        }
    }

    // 激活
    rm->initialiseAllResourceGroups(true);

    Ogre::Hlms* hlms = mpRoot->getHlmsManager()->getHlms(Ogre::HLMS_PBS);
    OGRE_ASSERT_HIGH(dynamic_cast<Ogre::HlmsPbs*>(hlms));
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>(hlms);
    try
    {
        hlmsPbs->loadLtcMatrix();
    }
    catch (Ogre::FileNotFoundException& e)
    {
        mpLogHandle->logMessage(e.getFullDescription(), Ogre::LML_CRITICAL);
        mpLogHandle->logMessage("WARNING: LTC matrix textures could not be loaded. Accurate specular IBL reflections "
            "and LTC area lights won't be available or may not function properly!"
        );
    }
}

void OgreWidget::setupCompositor()
{
    Ogre::CompositorManager2* manager = mpRoot->getCompositorManager2();
    const Ogre::String workspaceName("DefaultWorkspace");
    if (manager->hasWorkspaceDefinition(workspaceName))
        return;

    manager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue(0.2, 0.4, 0.6, 1));
    mpCompositor = manager->addWorkspace(mpDefaultSceneManager, mpRenderWindow->getTexture(), mpDefaultCamera, workspaceName, true);
}

void OgreWidget::mousePressEvent(QMouseEvent* event)
{
    setFocus();
    mpCameraHandle->mousePressed(event);
}

void OgreWidget::mouseReleaseEvent(QMouseEvent* event)
{
    mpCameraHandle->mouseReleased(event);
}

void OgreWidget::mouseMoveEvent(QMouseEvent* event)
{
    mpCameraHandle->mouseMoved(event);
}

void OgreWidget::wheelEvent(QWheelEvent* event)
{
    mpCameraHandle->mouseWheelRolled(event);
}

void OgreWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_O && (event->modifiers() & Qt::ControlModifier))
    {
        auto item = mpDefaultSceneManager->createItem("knot.mesh",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::SCENE_DYNAMIC);
        item->setDatablockOrMaterialName("snow");
        auto node = mpDefaultSceneManager->getRootSceneNode()->createChildSceneNode();

        node->attachObject(item);
        event->accept();
    }

    if(!event->isAccepted())
        mpCameraHandle->keyPressed(event);
}

void OgreWidget::keyReleaseEvent(QKeyEvent* event)
{
    mpCameraHandle->keyReleased(event);
}

void OgreWidget::resizeEvent(QResizeEvent* event)
{
    super::resizeEvent(event);

    const QSize& size = event->size();
    if (mpRenderWindow)
    {
        mpRenderWindow->windowMovedOrResized();
        mpRenderWindow->requestResolution(size.width(), size.height());
    }

    if (mpDefaultCamera && size.height() != 0)
        mpDefaultCamera->setAspectRatio(size.width() * 1.0f / size.height());
}

QPaintEngine* OgreWidget::paintEngine() const
{
    return nullptr;
}
