cmake_minimum_required(VERSION 3.5)

project(OgreJulyBlueprintKachow VERSION 0.0.1 LANGUAGES CXX)

include(./cmake/RestoreOrDownload.cmake)

# 配置选项
option(BUILD_SHARED "Build shared library" ON)
# set(CMAKE_BUILD_TYPE Debug|Release|MinSizeRel|RelWithDebInfo)

# 设置存放路径
set(OUTPUT_BASE_DIR "${CMAKE_BINARY_DIR}/output")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/bin)

#下载源码
option(OGREDEPS_BUILD_SHADERC "ogre-next-dep中编译shaderc的使能" OFF)
set(OGREDEPS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ogre-next-deps")
set(OGRE_DEP_STORE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/store/ogre-next-deps.tar.gz")
rod("ogre-dep" ${OGREDEPS_SOURCE_DIR} ${OGRE_DEP_STORE_FILE} "--recurse-submodules --shallow-submodules git@github.com:OGRECave/ogre-next-deps.git")

option(OGRE_BUILD_SAMPLES2 "ogre-next中编译用例的使能" OFF)
set(OGRE_BINARY_DIR "${OUTPUT_BASE_DIR}")
set(OGRE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ogre-next")
set(OGRE_STORE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/store/ogre-next.tar.gz")
rod("ogre-next" ${OGRE_DIR} ${OGRE_STORE_FILE} "git@github.com:OGRECave/ogre-next.git")

## 更新输出路径，统一由外侧调控
set(PATTERNS "OGRE_BINARY_DIR" "CMAKE_ARCHIVE_OUTPUT_DIRECTORY" "CMAKE_LIBRARY_OUTPUT_DIRECTORY" "CMAKE_RUNTIME_OUTPUT_DIRECTORY")
file(READ "${OGRE_DIR}/CMakeLists.txt" OgreCMakeLists)
foreach(PATTERN ${PATTERNS})
    string(REGEX REPLACE "\n(set\\(${PATTERN}.*\\))" "\n# \\1" OgreCMakeListsModified "${OgreCMakeLists}")
    set(OgreCMakeLists "${OgreCMakeListsModified}")
endforeach()
file(WRITE "${OGRE_DIR}/CMakeLists.txt" "${OgreCMakeLists}")

# 拷贝rapidjson
set(RAPID_JSON_IN_ORGR "${OGRE_DIR}/OgreMain/include/rapidjson")
if(NOT EXISTS ${RAPID_JSON_IN_ORGR})
    file(COPY 
        ${OGREDEPS_SOURCE_DIR}/src/rapidjson/include/
        DESTINATION 
        ${OGRE_DIR}/OgreMain/include/rapidjson)
    message("拷贝rapidjson到OgreMain")
endif()

# 设置c++版本
# 5555 FreeImage中使用了auto_ptr，而这个在c++17被砍掉了
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 超帅的，解决了vs中不将target_include_directories头文件路径添加到“附加包含目录”中的问题
set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

# 如果无法正常找到Qt5就开启下面的注释，并将路径更新
if(NOT DEFINED ENV{QT5_DIR})
    # message(FATAL_ERROR "未能找到QT5安装路径,请设置'QT5_DIR',他的路径为bin目录的上一层")
    set(QT5_DIR "C:/Qt/Qt5.12.8/5.12.8/msvc2017_64")
    message(STATUS "QT5_DIR 已更新")
endif()
set(CMAKE_PREFIX_PATH ${QT5_DIR})

if(NOT BUILD_SHARED)
    add_compile_definitions(BK_STATIC_LIB)
endif()

# ogre-next编译
add_subdirectory(ogre-next-deps)
add_subdirectory(ogre-next)

# 蓝图绘制的核心，基于QGraphics系列的封装
add_subdirectory(BlueprintKernal)
add_subdirectory(OgreJulyBlueprint)