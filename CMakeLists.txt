cmake_minimum_required(VERSION 3.5)

project(OgreJulyBlueprintKachow VERSION 0.0.1 LANGUAGES CXX)

# 配置选项
option(BUILD_SHARED "Build shared library" ON)
# set(CMAKE_BUILD_TYPE Debug|Release|MinSizeRel|RelWithDebInfo)

# 设置c++版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 超帅的，解决了vs中不将target_include_directories头文件路径添加到“附加包含目录”中的问题
set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

# 如果无法正常找到Qt5就开启下面的注释，并将路径更新
if(NOT DEFINED ENV{QT5_DIR})
    set(QT5_DIR "C:/Qt/Qt5.12.8/5.12.8/msvc2017_64")
    set(CMAKE_PREFIX_PATH ${QT5_DIR})
    message(STATUS "QT5_DIR 已更新")
endif()

# 设置存放路径
set(OUTPUT_BASE_DIR "${CMAKE_BINARY_DIR}/output")
if(NOT ${CMAKE_BUILD_TYPE})
    set(OUTPUT_BASE_DIR "${OUTPUT_BASE_DIR}/${CMAKE_BUILD_TYPE}")
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/bin)

if(NOT BUILD_SHARED)
    add_compile_definitions(BK_STATIC_LIB)
endif()

# 蓝图绘制的核心，基于QGraphics系列的封装
add_subdirectory(BlueprintKernal)
add_subdirectory(OgreJulyBlueprint)