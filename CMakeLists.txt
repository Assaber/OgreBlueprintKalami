cmake_minimum_required(VERSION 3.5)

project(OgreBlueprintKalami VERSION 0.0.1 LANGUAGES CXX)

# include(./cmake/RestoreOrDownload.cmake)

option(BUILD_SHARED "Build shared library" ON)
option(INCLUDE_INNER_PARTICLE_CARD "The particle cards in the tool are enabled" ON)
# set(CMAKE_BUILD_TYPE Debug|Release|MinSizeRel|RelWithDebInfo)

if(NOT MSVC)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

# Output dir
set(OUTPUT_BASE_DIR "${CMAKE_BINARY_DIR}/output")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/bin)
set(OGRE_ARCHIVE_OUTPUT ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(OGRE_LIBRARY_OUTPUT ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(OGRE_RUNTIME_OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Special Modifications
option(OGREDEPS_BUILD_SHADERC "Build shaderc" OFF)
set(OGREDEPS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ogre-next-deps")
# set(OGRE_DEP_STORE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/store/ogre-next-deps.tar.gz")
# rod("ogre-dep" ${OGREDEPS_SOURCE_DIR} ${OGRE_DEP_STORE_FILE} "--recurse-submodules --shallow-submodules git@github.com:OGRECave/ogre-next-deps.git")

option(OGRE_BUILD_SAMPLES2 "Build samples" OFF)
set(OGRE_BINARY_DIR "${OUTPUT_BASE_DIR}")
set(OGRE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ogre-next")
# set(OGRE_STORE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/store/ogre-next.tar.gz")
# rod("ogre-next" ${OGRE_DIR} ${OGRE_STORE_FILE} "git@github.com:OGRECave/ogre-next.git")

## Change build output dir: Each project is finally exported to the same place
set(PATTERNS "OGRE_BINARY_DIR" "CMAKE_ARCHIVE_OUTPUT_DIRECTORY" "CMAKE_LIBRARY_OUTPUT_DIRECTORY" "CMAKE_RUNTIME_OUTPUT_DIRECTORY")
file(READ "${OGRE_DIR}/CMakeLists.txt" OgreCMakeLists)
foreach(PATTERN ${PATTERNS})
    string(REGEX REPLACE "\n(set\\(${PATTERN}.*\\))" "\n# \\1" OgreCMakeListsModified "${OgreCMakeLists}")
    set(OgreCMakeLists "${OgreCMakeListsModified}")
endforeach()
file(WRITE "${OGRE_DIR}/CMakeLists.txt" "${OgreCMakeLists}")

# Copy rapidjson
set(RAPID_JSON_IN_ORGR "${OGRE_DIR}/OgreMain/include/rapidjson")
if(NOT EXISTS ${RAPID_JSON_IN_ORGR})
    file(COPY 
        ${OGREDEPS_SOURCE_DIR}/src/rapidjson/include/
        DESTINATION 
        ${OGRE_DIR}/OgreMain/include/rapidjson)
    message("Copy rapidjson to OgreMain")
endif()
## Redirect rapidjson location
set(Rapidjson_INC_SEARCH_PATH "${OGRE_DIR}/OgreMain/include")

# Set c++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fixed an issue in VS where target_include_directories header file path was not added to the "Attach Include Directory".
set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

if(NOT BUILD_SHARED)
    add_compile_definitions(BK_STATIC_LIB)
endif()

# Allows the creation of project folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build ogre-next
add_subdirectory(ogre-next-deps)
add_subdirectory(ogre-next)

# Build Kalami
add_subdirectory(BlueprintKernal)
add_subdirectory(OgreJulyBlueprint)
