cmake_minimum_required(VERSION 3.5)

project(BlueprintKernal)

include(../cmake/AutoAddSources.cmake)

find_package(Qt5 COMPONENTS Widgets Gui Core REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# qt5_add_resources(QRC_FILES a.qrc)

set(FLAGS)
if(BUILD_SHARED)
    set(FLAGS SHARED ${FLAGS})
else()
    set(FLAGS STATIC ${FLAGS})
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_library(${PROJECT_NAME} ${FLAGS} ${PROJECT_SOURCES})
add_sources(${PROJECT_NAME})

if(BUILD_SHARED)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets)

    if (MSVC)
        target_compile_definitions(${PROJECT_NAME} PRIVATE BLUEPRINT_KERNAL_EXPORT)
    endif()
endif()

target_include_directories( ${PROJECT_NAME} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${QT5_DIR}/include"
    "${QT5_DIR}/include/QtCore"
    "${QT5_DIR}/include/QtGui"
    "${QT5_DIR}/include/QtWidgets"
)