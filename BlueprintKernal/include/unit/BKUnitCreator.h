#pragma once
#include <memory>
#include "unit/BKUnit.h"

class BKUnitCreator
{
public:
    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<BKUnit, T>>>
    T* create(Args &&...args)
    {
        auto ptr = new T(std::forward<Args>(args)...);
        return ptr;
    }
};