#pragma once
#include "common_obk.h"
#include <QString>

class AppConfig
{
public:
    static AppConfig* getSingleton();

public:
    RenderSystem getRenderSystem();
    void setRenderSystem(RenderSystem rs);

    QString getMediaLocation();
    void setMediaLocation(const QString& dir);

public:
    // export
    bool saveConfig();
    // import
    bool loadConfig();

public:
    static RenderSystem getRenderSystemByName(const QString& name);
    static QString getRenderSystemName(RenderSystem rs);
    static QStringList getRenderSystemNames();

protected:
    AppConfig();

private:
    bool standardDirPath(QString& dir);

private:
    static AppConfig* mpInstance;
    static constexpr char* mConfigPath = "./config.obk";

    RenderSystem mRenderSys = RenderSystem::Default;
    QString mstrMediaLocation = "";

    static QMap<RenderSystem, QString> mRenderSystem2Name;
};