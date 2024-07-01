#include "AppConfig.h"
#include <QDir>
#include <QSettings>

#define SETTING_CHECK(condition, function, ret) if(condition) { \
    function;\
}\
ret = ret && (condition);

AppConfig* AppConfig::mpInstance = nullptr;

QMap<RenderSystem, QString> AppConfig::mRenderSystem2Name = {
    {RenderSystem::Direct3D, "Direct3D11 Rendering Subsystem"},
    {RenderSystem::Vulkan, "Vulkan Rendering Subsystem"},
    {RenderSystem::OpenGL, "OpenGL 3+ Rendering Subsystem"},
};

AppConfig* AppConfig::getSingleton()
{
    if (!mpInstance) {
        mpInstance = new AppConfig();
    }

    return mpInstance;
}

AppConfig::AppConfig() {}

RenderSystem AppConfig::getRenderSystem()
{
    return mRenderSys;
}

void AppConfig::setRenderSystem(RenderSystem rs)
{
    mRenderSys = rs;
}

QString AppConfig::getMediaLocation()
{
    return mstrMediaLocation;
}

void AppConfig::setMediaLocation(const QString& dir)
{
    QString d = dir;
    if (!standardDirPath(d))
        return;

    mstrMediaLocation = d;
}

RenderSystem AppConfig::getRenderSystemByName(const QString& name)
{
    return mRenderSystem2Name.key(name, RenderSystem::Default);
}

QString AppConfig::getRenderSystemName(RenderSystem rs)
{
    return mRenderSystem2Name.value(rs, "");
}

QStringList AppConfig::getRenderSystemNames()
{
    return mRenderSystem2Name.values();
}

bool AppConfig::loadConfig()
{
    QFile config(mConfigPath);
    if (!config.exists())
        return false;

    if (!config.open(QIODevice::ReadOnly))
        return false;

    config.close();
    

    QSettings settings(mConfigPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("App");
    
    bool ret = true;

    QString renderSystem = settings.value("RenderSystem", "").toString();
    SETTING_CHECK(!renderSystem.isEmpty(), mRenderSys = static_cast<RenderSystem>(renderSystem.toInt()), ret);

    QString mediaLocation = settings.value("MediaLocation", "").toString();
    SETTING_CHECK(!mediaLocation.isEmpty() && standardDirPath(mediaLocation), mstrMediaLocation = mediaLocation , ret);

    return ret;
}

bool AppConfig::saveConfig()
{
    //check
    if (!standardDirPath(mstrMediaLocation))
        return false;

    QSettings settings(mConfigPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("App");
    settings.setValue("RenderSystem", QString::number(static_cast<int>(mRenderSys)));
    settings.setValue("MediaLocation",mstrMediaLocation);
    settings.endGroup();

    return true;
}

bool AppConfig::standardDirPath(QString& dir)
{
    QDir d(dir);
    if (!d.exists())
        return false;

    dir = d.absolutePath() + "/";
}