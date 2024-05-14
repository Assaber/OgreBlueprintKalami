#pragma once
#include "container/BKCard.h"
#include <QFileSystemWatcher>
#include <QObject>

class BKLineEdit;
class BKAnchor;
class ReadFileCard : public QObject, public BKCard
{
    CREATE_CARD_FACTORY(ReadFileCard)
public:
    ReadFileCard(QObject* parent = nullptr);

private:
    bool openFile(const QVariant& data);
    bool reloadFile(const QVariant& data);
    void loadFile(const QString& path);
    bool updateWatcherStatus(const QVariant& data);
    void fileChanged(const QString& path);

private:
    // 文件路径编辑栏
    BKLineEdit* mpFilepathEdit = nullptr;
    // 输出锚点
    BKAnchor* mpOutputAnchor = nullptr;
    // 文件监视器
    QFileSystemWatcher mWatcher;
    // 文件监视器使能
    bool mbWatchEnable = false;
    // 最后一个打开的文件路径
    QString mstrLastFilepath = "";
};