#include "BKCreator.h"
#include "container/BKCard.h"
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QDebug>

CardCreatorPtr BKCreator::getCreator(const char* name, const char* group /*= "Default"*/)
{
    auto gi = BKCreatorMenu::mRegistItems.find(group);
    if (gi == BKCreatorMenu::mRegistItems.end())
        return nullptr;

    const auto& cards = gi->second;
    auto ci = cards.find(name);

    return ci != cards.end() ? ci->second : nullptr;
}

BKCreatorMenu::BKCreatorMenu(BlueprintLoader* loader)
    : QComboBox(loader)
    , mpLoader(loader)
    , mpInput(new QLineEdit(this))
    , mpCompleter(new QCompleter(this))
    , mpModel(new QStringListModel(this))
{
    this->setFixedSize(230, 35);
    updateListItems();

    mpInput->setAcceptDrops(false);
    mpCompleter->setCaseSensitivity(Qt::CaseInsensitive);       //大小写不敏感
    mpCompleter->setFilterMode(Qt::MatchRecursive);             //匹配规则：包含则匹配

    setCompleter(mpCompleter);
    setEditable(true);
    setLineEdit(mpInput);

    QObject::connect(this, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, [&](const QString& text) {
        do
        {
            QRegExp rx("^\\[(.*)\\] (.*)$");
            int pos = rx.indexIn(text);
            if (pos < 0) {
                qCritical() << QString("[错误] 菜单构建与正则不匹配。[文件]%1, [行数]%2").arg(__FILE__).arg(__LINE__);
                break;
            }

            QStringList list = rx.capturedTexts();

            std::string group = list[1].toStdString();
            std::string card = list[2].toStdString();

            CardCreatorPtr creator = BKCreator::getCreator(card.c_str(), group.c_str());
            if (creator) {
                mpLoader->setDarling(creator(mpLoader));
            }


        } while (false);

        hide();
        });
}

void BKCreatorMenu::updateListItems()
{
    clear();

    QStringList items;
    for (const auto& item : mRegistItems)
        for (const auto& child : item.second)
            items << QString("[%1] %2").arg(item.first.c_str()).arg(child.first.c_str());       //注意这里的格式需要与正则过滤匹配

    mpModel->removeRows(0, mpModel->rowCount());
    mpModel->setStringList(items);
    mpCompleter->setModel(mpModel);
    addItems(items);
}

std::map<std::string, std::map<std::string, CardCreatorPtr>> BKCreatorMenu::mRegistItems;
