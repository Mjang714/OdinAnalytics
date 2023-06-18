cmake_minimum_required(VERSION 3.21)

##
# How to build:
#
# *nix (ELF 64-bit):
#
#   cmake -S . -B build && cmake --build build -j
#
# Windows:
#   arch: Win32, x64
#
#   cmake -S . -B build_windows_[arch] -A [arch] && ^
#   cmake build_windows_[arch] -j
#
# Build driver scripts build.bat and build.sh will be contributed later. To
# build shared libraries, add -DBUILD_SHARED_LIBS to the cmake config command.
#
# Note that ODIN_BOOST_ROOT and ODIN_MAGIC_ENUM_ROOT need to be set either in
# the environment or passed using -D to the cmake config command.
#

# major, minor, patch version also defined as macros in OdinAnalytics
set(ODIN_MAJOR_VERSION 0)
set(ODIN_MINOR_VERSION 1)
set(ODIN_PATCH_VERSION 0)

# project name, in case we want to change it later
set(ODIN_PROJECT_NAME OdinAnalytics)

project(
    ${ODIN_PROJECT_NAME}
    VERSION ${ODIN_MAJOR_VERSION}.${ODIN_MINOR_VERSION}.${ODIN_PATCH_VERSION}
    DESCRIPTION "Nordic quantitative library sandbox"
    LANGUAGES C CXX
)

# check if we are using single of multi-config generator
get_property(ODIN_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# default build type Debug for single-config generators
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# C/C++ standards + require compiler support
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# eliminate per-configuration subdirs from being appended to build artifact
# output dirs for multi-config generators, e.g. Visual Studio
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:>
)
set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:>
)
set(
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:>
)

# update CMake module path to allow inclusion of custom CMake code
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# set custom compile options
include(ConfigCompile)
# find dependencies
include(FindDeps)

# path to include directory (include is a reserved CMake keyword)
# TODO: consider using separate include directory, commented out for now
# set(ODIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
# include_directories(${ODIN_INCLUDE_DIR})
# path to src directory (we use this later when adding files to test runner)
set(ODIN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# also used as include directory, consider using separate ./include dir
include_directories(${ODIN_SOURCE_DIR})

# enables test discovery if you wish to use ctest
enable_testing()

add_subdirectory(${ODIN_SOURCE_DIR})
