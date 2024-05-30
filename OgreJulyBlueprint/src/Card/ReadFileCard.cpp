#include "ReadFileCard.h"
#include "BKCreator.h"
#include "container/BKCell.h"
#include "BKCreator.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKPushButton.h"
#include "unit/BKLineEdit.h"
#include "unit/BKCheckBox.h"

#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>


ReadFileCard::ReadFileCard(QObject* parent)
    : QObject(parent)
    , BKCard()
{
    setTitle("读取文件");

    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, BKAnchor::String);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();
    BKLabel* outputLabel = BKCreator::create<BKLabel>();
    outputLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    outputLabel->setText("输出");
    outputLabel->setMinWidth(160);
    outputCell->append(outputLabel, false);

    BKCell* watcherCell = BKCreator::create(BKAnchor::AnchorType::None);
    BKLabel* watcherLabel = BKCreator::create<BKLabel>();
    watcherLabel->setText("自动监听");
    watcherCell->append(watcherLabel, false);
    BKCheckBox* watcherCheckBox = BKCreator::create<BKCheckBox>();
    watcherCheckBox->setChecked(false);
    watcherCheckBox->setDataChangeCallback(std::bind(&ReadFileCard::updateWatcherStatus, this, std::placeholders::_1));
    watcherCell->append(watcherCheckBox);

    BKCell* flTitleCell = BKCreator::create(BKAnchor::AnchorType::None);
    BKLabel* flTitleLabel = BKCreator::create<BKLabel>();
    flTitleLabel->setText("文件加载");
    flTitleCell->append(flTitleLabel);

    BKCell* fileLoaderCell = BKCreator::create(BKAnchor::AnchorType::None);
    mpFilepathEdit = BKCreator::create<BKLineEdit>();
    mpFilepathEdit->setEditable(false);
    fileLoaderCell->append(mpFilepathEdit);
    BKPushButton* openFileBtn = BKCreator::create<BKPushButton>();
    openFileBtn->setText("...");
    openFileBtn->setFixedSize({ 25, 25 });
    openFileBtn->setDataChangeCallback(std::bind(&ReadFileCard::openFile, this, std::placeholders::_1));
    fileLoaderCell->append(openFileBtn);
    BKPushButton* reloadBtn = BKCreator::create<BKPushButton>();
    reloadBtn->setText("○");
    reloadBtn->setFixedSize({ 25, 25 });
    reloadBtn->setDataChangeCallback(std::bind(&ReadFileCard::reloadFile, this, std::placeholders::_1));
    fileLoaderCell->append(reloadBtn);

    _pack({
        outputCell,
        watcherCell,
        flTitleCell,
        fileLoaderCell,
        });

    QObject::connect(&mWatcher, &QFileSystemWatcher::fileChanged, this, &ReadFileCard::fileChanged);
}

QVariant ReadFileCard::getCurrentCardValue()
{
    return mstrData;
}

bool ReadFileCard::openFile(const QVariant& data)
{
    QString filepath = QFileDialog::getOpenFileName();
    if (filepath.isEmpty())
        return true;

    if(!mstrLastFilepath.isEmpty())
        mWatcher.removePath(mstrLastFilepath);

    mstrLastFilepath = filepath;
    mWatcher.addPath(filepath);
    mpFilepathEdit->setText(filepath);
    return true;
}

bool ReadFileCard::reloadFile(const QVariant& data)
{
    QString filepath = mpFilepathEdit->getCurrentText();
    if (filepath.isEmpty())
    {
        QMessageBox::critical(nullptr, "错误", "文件路径为空，请先打开文件（左边那个钮）然后再加载...");
        return true;
    }

    if (!QFileInfo(filepath).exists())
    {
        QMessageBox::critical(nullptr, "错误", "文件无法打开，请确定文件存在，且拥有读取权限");
        return true;
    }

    loadFile(filepath);
    mpOutputAnchor->dataChanged(getCurrentCardValue());

    return true;
}

void ReadFileCard::loadFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "文件打开失败..." << path;
    }

    mstrData = QString::fromUtf8(file.readAll());
    file.close();
}

bool ReadFileCard::updateWatcherStatus(const QVariant& data)
{
    mbWatchEnable = data.toBool();
    return true;
}

void ReadFileCard::fileChanged(const QString& path)
{
    if (path.compare(mpFilepathEdit->getCurrentText()) == 0)
    {
        if (mbWatchEnable) {
            loadFile(path);
            mpOutputAnchor->dataChanged(getCurrentCardValue());
        }
    }
    else
    {
        mWatcher.removePath(path);
    }
}
