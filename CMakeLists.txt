cmake_minimum_required(VERSION 3.21)

##
# How to build:
#
# *nix (ELF 64-bit):
#
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
#
# Windows:
#
#   cmake -S . -B build_windows_(x86|x64) -A (Win32|x64) && ^
#   cmake --build build_windows_(x86|x64) --config Release -j
#
# FIXME: Below content is all out of date
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
#       magic_enum install root, i.e. should contain include directory.
#
#   ODIN_GTEST_ROOT
#       Google Test install root. Can use GTEST_ROOT instead and does not need
#       to be set if Google Test is already installed in the system
#
#   ODIN_EXCELSDK_ROOT
#       Excel SDK install root, i.e. should contain INCLUDE, LIB directories.
#       This need only be available for Windows builds and the user must use
#       SAMPLES\MAKE.bat to build the samples, in particular frmwrk32.lib.
#
# These can be defined either in the environment or passed to the CMake config
# command directly by using the -D flag.
#

project(
    OdinAnalytics
    VERSION 0.1.0
    DESCRIPTION "Nordic quantitative library sandbox"
    LANGUAGES C CXX
)

# find Git for build hash
find_package(Git 2.34)
if(Git_FOUND)
    message(STATUS "Git version: ${GIT_VERSION_STRING}")
else()
    message(STATUS "Git version: None")
endif()

# check if we are using single of multi-config generator
get_property(ODIN_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(ODIN_MULTI_CONFIG)
    message(STATUS "Build config: Multi")
else()
    # default build type Debug for single-config generators
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()
    message(STATUS "Build config: ${CMAKE_BUILD_TYPE}")
endif()

# C/C++ standards + require compiler support
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# write the binary objects into the top-level build directory. multi-config
# generators like Visual Studio will add per-config subdirectories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# build shared libraries by default
option(BUILD_SHARED_LIBS "Build shared libraries." ON)
option(ODIN_BUILD_RELEASE "Build official release" OFF)

# indicate if building shared libs
if(BUILD_SHARED_LIBS)
    message(STATUS "Build libraries: Shared")
else()
    message(STATUS "Build libraries: Static")
endif()

# indicate if using unity build setting or not
# note: unity builds decrease compile time and help reduce CPU compilation load
# but result in larger individual TUs due to aggregation. incremental build
# also suffers so it's best to use unity builds for clean builds, e.g. on CI
if(CMAKE_UNITY_BUILD)
    message(STATUS "Build unity: Yes")
else()
    message(STATUS "Build unity: No")
endif()

# update CMake module path to allow inclusion of custom CMake code
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CTest)

# find Boost [headers]
# TODO: we can decide to upgrade minimum Boost version later if necessary
# find using OLD policy so working with non-CMake installs still works
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0167 OLD)
endif()
find_package(Boost 1.71 REQUIRED)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    cmake_policy(POP)
endif()
message(STATUS "Boost version: ${Boost_VERSION}")

# find Magic Enum
find_package(magic_enum 0.8.1 REQUIRED)
message(STATUS "Magic Enum version: ${magic_enum_VERSION}")

# find GoogleTest
find_package(GTest 1.8)
if(GTest_FOUND)
    message(STATUS "GoogleTest version: ${GTest_VERSION}")
else()
    message(STATUS "GoogleTest version: None")
endif()

# look for Excel 2013 XLL SDK
if(WIN32)
    find_package(XLLSDK 15.0)
endif()
if(XLLSDK_FOUND)
    message(STATUS "XLL SDK version: ${XLLSDK_VERSION}")
else()
    message(STATUS "XLL SDK version: None")
endif()

# set compiler-specific definitions and options
# note: do this after looking for dependencies to avoid influencing deps
if(MSVC)
    # disable warnings about insecure functions, C code is naturally unsafe
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(
        # preferred warning level + enable parallel compilaton. the CMake
        # -j, --parallel flag only builds *projects* in parallel
        # TODO: should put this back up to /Wall when possible. do *not* remove
        # the explicitly disabled warnings for now
        /W4 /MP
        # typically don't care if unreferenced inline functions are removed
        /wd4514
        # silence warnings about padding (we don't care for this project)
        /wd4820
        # for Google Test ::testing::Test subclasses, these are reported a lot,
        # i.e. for implicitly deleted copy/move ctor + operator=
        /wd4625 /wd4626 /wd5026 /wd5027
        # Google Test gtest-param-util.h triggers these warnings (braced
        # initialization list eval order, Spectre mitigation)
        /wd4868 /wd5045
        # /Wall enables excessive warnings about automatic inline expansion
        /wd4710 /wd4711
        # /Wall enables warning about 32-bit floats being stored in memory,
        # which is performance loss compared to storing in register
        /wd4738
        # /Od applied by default when using Debug configuration
    )
    # if building as DLL, C4251 is emitted a lot. this is because exporting C++
    # classes from DLLs is fraught with ABI issues, but this is fine for more
    # recent versions of MSVC which are all ABI-compatible.
    if(BUILD_SHARED_LIBS)
        add_compile_options(/wd4251)
    endif()
else()
    add_compile_options(
        -Wall
        # -O0 is default optimization level anyways, Clang ignores -ggdb
        $<$<NOT:$<CONFIG:Release>>:-ggdb> $<IF:$<CONFIG:Release>,-O3,-O0>
    )
endif()

# major, minor, patch version components
set(ODIN_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(ODIN_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(ODIN_PATCH_VERSION ${PROJECT_VERSION_PATCH})
# obtain short Git revision
if(Git_FOUND)
    execute_process(
        COMMAND git rev-parse --short HEAD
        RESULT_VARIABLE git_rev_res
        OUTPUT_VARIABLE ODIN_REVISION
        ERROR_VARIABLE git_rev_err
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )
    # note: should not fail
    if(git_rev_res)
        message(FATAL_ERROR "git rev-parse --short HEAD error: ${git_rev_err}")
    endif()
endif()
# set build info. this is the short revision or DEV for non-release builds
if(ODIN_BUILD_RELEASE)
    set(ODIN_BUILD_INFO "")
else()
    if(Git_FOUND)
        set(ODIN_BUILD_INFO ${ODIN_REVISION})
    else()
        set(ODIN_BUILD_INFO "DEV")
    endif()
endif()
# set version string
set(ODIN_VERSION "${ODIN_MAJOR_VERSION}.${ODIN_MINOR_VERSION}.${ODIN_PATCH_VERSION}")
if(NOT ODIN_BUILD_RELEASE)
    string(APPEND ODIN_VERSION "-${ODIN_BUILD_INFO}")
endif()

# generate Odin version header
set(ODIN_VERSION_H include/oa/version.h)
# variables consumed:
#
#   ODIN_MAJOR_VERSION
#   ODIN_MINOR_VERSION
#   ODIN_PATCH_VERSION
#   ODIN_VERSION
#   ODIN_BUILD_INFO
#
configure_file(${ODIN_VERSION_H}.in ${ODIN_VERSION_H} @ONLY LF)
message(STATUS "Generated ${ODIN_VERSION_H}")

# path to include directory (include is a reserved CMake keyword)
set(ODIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
# path to src directory (we use this later when adding files to test runner)
# TODO: stop using this as part of target include paths later on
set(ODIN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src)
