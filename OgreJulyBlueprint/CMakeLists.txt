cmake_minimum_required(VERSION 3.5)

project(OgreJulyBlueprint)

include(../cmake/AutoAddSources.cmake)

find_package(Qt5 COMPONENTS Widgets Gui Core REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 打印Qt5安装路径
get_target_property(Qt5Core_LOCATION Qt5::Core LOCATION)
get_filename_component(Qt5Core_DIR ${Qt5Core_LOCATION} DIRECTORY)
get_filename_component(Qt5_INSTALL_DIR ${Qt5Core_DIR} DIRECTORY)
message(STATUS "Qt5 is installed in: ${Qt5_INSTALL_DIR}")

#qt5_add_resources(QRC_FILES a.qrc)
set(FLAGS)
if(MSVC)
    set(FLAGS WIN32)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(
    include
    include/Card
    include/Engine
    ${CMAKE_BINARY_DIR}/ogre-next/include
	${OUTPUT_BASE_DIR}/include
    ${OGRE_DIR}
    ${OGRE_DIR}/OgreMain/include
    ${OGRE_DIR}/Components/Hlms/Common/include
    ${OGRE_DIR}/Components/Hlms/Pbs/include
    ${OGRE_DIR}/Components/Hlms/Unlit/include
    )

if(INCLUDE_INNER_PARTICLE_CARD)
    include_directories(${OGRE_DIR}/PlugIns/ParticleFX/include)
    add_compile_definitions(OBK_INCLUDE_PARTICLE_PLUGIN)
    message(STATUS "The particle cards are activated")
endif()

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

if(INCLUDE_INNER_PARTICLE_CARD)
    add_dependencies(${PROJECT_NAME} Plugin_ParticleFX)
    target_link_libraries(${PROJECT_NAME} PRIVATE Plugin_ParticleFX)
endif()


if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_INSTALL_DIR}/bin;$\{PATH\}")
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$\(OutDir\)")
endif()

# 拷贝
set(OGRE_SAMPLE_COMMON "${OUTPUT_BASE_DIR}/media")
if(NOT EXISTS ${OGRE_SAMPLE_COMMON})
    file(COPY 
        ${OGRE_DIR}/Samples/Media/2.0/scripts/materials/Common
        DESTINATION 
        ${OGRE_SAMPLE_COMMON})
    file(COPY 
        ${OGRE_DIR}/Samples/Media/Hlms
        DESTINATION 
        ${OGRE_SAMPLE_COMMON})
	file(COPY 
        ${CMAKE_SOURCE_DIR}/media/models
        DESTINATION 
        ${OGRE_SAMPLE_COMMON})
    file(COPY 
        ${CMAKE_SOURCE_DIR}/media/material
        DESTINATION 
        ${OGRE_SAMPLE_COMMON})
	file(COPY 
        ${CMAKE_SOURCE_DIR}/media/texture
        DESTINATION 
        ${OGRE_SAMPLE_COMMON})
    message("拷贝必要资源")
endif()