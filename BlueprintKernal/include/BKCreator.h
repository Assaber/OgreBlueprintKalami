#pragma once
#include "global_blueprint_kernal.h"
#include <memory>

class BKCell;
class BKUnit;
class BlueprintLoader;
class BKCreator
{
    using CardCreatorPtr = BKCard * (*)(BlueprintLoader* loader);
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
    static void registCard()
    {
        mRegistItems.insert({ T::Factory::_cardName, T::Factory::createOne });
    }

    static CardCreatorPtr getCreator(const char* name)
    {
        auto itor = mRegistItems.find(name);
        return itor != mRegistItems.end() ? itor->second : nullptr;
    }

private:
    static _BlueprintKernalExport std::map<std::string, CardCreatorPtr> mRegistItems;
};