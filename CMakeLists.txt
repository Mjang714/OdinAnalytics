cmake_minimum_required(VERSION 3.21)

##
# How to build:
#
# *nix (ELF 64-bit):
#
#   cmake -S . -B build && cmake --build build -j
#
# Windows:
#
#   cmake -S . -B build_windows_(x86|x64) -A (Win32|x64)) && ^
#   cmake build_windows_(x86|x64)) -j8
#
# Build driver scripts build.bat and build.sh will be contributed later. To
# build shared libs, add -DBUILD_SHARED_LIBS=1 to the cmake config command. To
# build static libs, add -DBUILD_SHARED_LIBS=0 instead.
#
# The following env/CMake variables can be used to find dependencies:
#
#   ODIN_BOOST_ROOT
#       Boost install root. Can use BOOST_ROOT instead and does not need to be
#       set if Boost is already installed in the system (e.g. through apt)
#
#   ODIN_MAGIC_ENUM_ROOT
#       magic_enum install root
#
#   ODIN_GTEST_ROOT
#       Google Test install root. Can use GTEST_ROOT instead and does not need
#       to be set if Google Test is already installed in the system
#
#   ODIN_EXCELSDK_ROOT
#       Excel SDK install root, i.e. should contain INCLUDE, LIB diretories.
#       This need only be available for Windows builds and the user must use
#       SAMPLES\MAKE.bat to build the samples, in particular frmwrk32.lib.
#
# These can be defined either in the environment or passed to the CMake config
# command directly by using the -D flag.
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

# write the binary objects into the top-level build directory. use of generator
# expression is to prevent multi-config generators, e.g. Visual Studio, from
# appending per-configuration subdirs to the specified output dir.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:>)

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
