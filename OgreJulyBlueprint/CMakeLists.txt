cmake_minimum_required(VERSION 3.5)

project(OgreJulyBlueprint)

include(../cmake/AutoAddSources.cmake)

find_package(Qt5 COMPONENTS Widgets Gui Core REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#qt5_add_resources(QRC_FILES a.qrc)
set(FLAGS)
if(MSVC)
    set(FLAGS WIN32)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(${PROJECT_NAME} ${FLAGS})
add_sources(${PROJECT_NAME} )

add_dependencies(${PROJECT_NAME} BlueprintKernal OgreMain OgreHlmsUnlit OgreHlmsPbs)

target_include_directories( ${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    BlueprintKernal
    OgreMain
    OgreHlmsUnlit
    OgreHlmsPbs
)

if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=${QT5_DIR}/bin;$\{PATH\}")
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$\(OutDir\)")
endif()