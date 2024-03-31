﻿#pragma once
#include <QEvent>

#if defined( __WIN32__ ) || defined( _WIN32 )
#   if defined( BK_STATIC_LIB )
#       define _BlueprintKernalExport
#   else
#       if defined( BLUEPRINT_KERNAL_EXPORT )
#           define _BlueprintKernalExport __declspec( dllexport )
#       else
#           if defined( __MINGW32__ )
#               define _BlueprintKernalExport
#           else
#               define _BlueprintKernalExport __declspec( dllimport )
#           endif
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _BlueprintKernalExport __attribute__ ((visibility("default")))
#else
#   define _BlueprintKernalExport
#endif 


static constexpr int AnchorMouseEvent = QEvent::User + 1001;
static constexpr int UpdateTopmostCardEvent = QEvent::User + 1002;

static constexpr int StandAloneUnitInUserData = 101;

class BlueprintLoader;
// 获取枚举值的上升类型
#define GET_ENUM_CLASS_UPWARD_VARIANT(x) (static_cast<std::underlying_type<decltype(x)>::type>(x))
#define GET_CARD_NAME(Class) #Class
#define CREATE_CARD_FACTORY(Class) \
private: \
friend class BlueprintLoader; \
friend class BKCreator; \
class Factory \
{ \
public: \
    static BKCard* createOne(BlueprintLoader* loader) { \
        return BKCreator::create<Class>(loader); \
    } \
    static constexpr char* _cardName = GET_CARD_NAME(Class); \
};

#define L_IMPL(x) x::Impl* l = mpImpl;