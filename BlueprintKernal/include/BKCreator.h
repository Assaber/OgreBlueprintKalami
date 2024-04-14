#pragma once
#include "global_blueprint_kernal.h"
#include <memory>
#include <map>
#include <QComboBox>

class BKCell;
class BKUnit;
class BKCard;
class BlueprintLoader;
class QStringListModel;

using CardCreatorPtr = BKCard* (*)(BlueprintLoader* loader);
class BKCreator
{
public:
    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<BKUnit, T>>>
    static T* create(Args &&...args)
    {
        auto ptr = new T(std::forward<Args>(args)...);
        return ptr;
    }

    template<typename... Args>
    static BKCell* create(Args &&...args)
    {
        auto ptr = new BKCell(std::forward<Args>(args)...);
        return ptr;
    }
    
    template<typename T, typename = std::enable_if_t<std::is_base_of_v<BKCard, T>>>
    static BKCard* create(BlueprintLoader* loader)
    {
        auto ptr = loader->_createUnit<T>();
        return ptr;
    }

    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<BKCard, T>>>
    static void registCard(const char* group = "Default")
    {
        auto& g = BKCreatorMenu::mRegistItems[group];
        g.insert({ T::Factory::_cardName, T::Factory::createOne });
    }

    static CardCreatorPtr getCreator(const char* name, const char* group = "Default");
};

class _BlueprintKernalExport BKCreatorMenu : public QComboBox
{
public:
    BKCreatorMenu(BlueprintLoader* loader);

private:
    void updateListItems();

private:
    BlueprintLoader* mpLoader = nullptr;
    QLineEdit* mpInput = nullptr;
    QCompleter* mpCompleter = nullptr;
    QStringListModel* mpModel = nullptr;

private:
    friend class BKCreator;
    static std::map<std::string, std::map<std::string, CardCreatorPtr>> mRegistItems;
};