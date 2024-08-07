﻿#pragma once
#include "global_blueprint_kernal.h"

class BKCell;
class BKCard;
class BlueprintLoader;
class BKUnit;
using CardCreatorPtr = BKCard * (*)(BlueprintLoader* loader);
class BKCreator
{
public:
    template<typename T, typename... Args, typename = typename std::enable_if<std::is_base_of<BKUnit, T>::value>::type>
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
};