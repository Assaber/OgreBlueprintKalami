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

    virtual QVariant getCurrentCardValue() override;

private:
    bool openFile(BKUnit* unit, const QVariant& data);
    bool reloadFile(BKUnit* unit, const QVariant& data);
    void loadFile(const QString& path);
    bool updateWatcherStatus(BKUnit* unit, const QVariant& data);
    void fileChanged(const QString& path);

private:
    BKLineEdit* mpFilepathEdit = nullptr;

    BKAnchor* mpOutputAnchor = nullptr;

    QFileSystemWatcher mWatcher;

    bool mbWatchEnable = false;

    QString mstrLastFilepath = "";

    QString mstrData = "";
};