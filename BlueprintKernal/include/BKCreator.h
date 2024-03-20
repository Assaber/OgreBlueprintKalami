#pragma once
#include <memory>
#include "unit/BKUnit.h"
#include "container/BKCell.h"

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
};

