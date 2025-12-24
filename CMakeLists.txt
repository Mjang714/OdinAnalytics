cmake_minimum_required(VERSION 3.23)

# see README.md for build + test instructions

project(
    OA
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

# Visual Studio generator specific info
string(FIND "${CMAKE_GENERATOR}" "Visual Studio" _is_vs_generator)
if(_is_vs_generator EQUAL 0)
    message(STATUS "Visual Studio platform: ${CMAKE_VS_PLATFORM_NAME}")
    message(STATUS "Visual Studio toolset: ${CMAKE_VS_PLATFORM_TOOLSET}")
endif()
unset(_is_vs_generator)

# update CMake module path to allow inclusion of custom CMake code
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CTest)

include(oa_pdb_install)

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

# find GoogleMock by detecting GTest::gmock target. GoogleTest and GoogleMock
# may be packaged separately on some Linux distros so GTest::gmock[_main] may
# not be defined if GoogleMock is not found in the find_package call
if(TARGET GTest::gmock)
    set(GMock_FOUND TRUE)
    message(STATUS "GoogleMock version: ${GTest_VERSION}")
else()
    set(GMock_FOUND FALSE)
    message(STATUS "GoogleMock version: None")
endif()

# look for Excel 2013 XLL SDK
if(WIN32)
    find_package(XLLSDK 15.0 COMPONENTS frmwrk32)
endif()
if(XLLSDK_FOUND)
    message(STATUS "XLL SDK version: ${XLLSDK_VERSION}")
    if(XLLSDK_64BIT)
        message(STATUS "XLL SDK 64-bit: Yes")
    else()
        message(STATUS "XLL SDK 64-bit: No")
    endif()
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

# print version info
message(STATUS "Build version: ${ODIN_VERSION}")

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

# project build directory relative to CMAKE_CURRENT_SOURCE_DIR
cmake_path(RELATIVE_PATH PROJECT_BINARY_DIR OUTPUT_VARIABLE ODIN_REL_BUILD_DIR)
# project build directory generator expression (for multi-config subdir)
set(
    ODIN_BUILD_DIR_GENEX
    "${PROJECT_BINARY_DIR}$<${ODIN_MULTI_CONFIG}:/$<CONFIG>>"
)
# project relative build directory generator expression
set(
    ODIN_REL_BUILD_DIR_GENEX
    "${ODIN_REL_BUILD_DIR}$<${ODIN_MULTI_CONFIG}:/$<CONFIG>>"
)
# custom target to copy static data to build output directory
# note: multi-config generators will have the per-config subdirectory
add_custom_target(
    oa_copy_static_data
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            static_data/calendars ${ODIN_BUILD_DIR_GENEX}/calendars
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Copying static data to ${ODIN_BUILD_DIR_GENEX}"
    VERBATIM
)

# oa_core: core header-only library
# FIXME: currently oa/rtti.h still requires Boost
# TODO: go through project targets to see which implicitly depend on this
add_library(oa_core INTERFACE)
target_include_directories(
    oa_core INTERFACE
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
# install rule for oa_core (provides just an IMPORTED target)
install(TARGETS oa_core EXPORT odin_exports)

add_subdirectory(src)
add_subdirectory(test)

# install rule for include/oa headers (excluding internal testing headers)
# note: this is currently incomplete and only provides core + config headers
install(
    DIRECTORY ${ODIN_INCLUDE_DIR}/oa
    TYPE INCLUDE
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h" PATTERN "testing*" EXCLUDE
)
# install rule for generated version.h header
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${ODIN_VERSION_H}
    DESTINATION include/oa
    COMPONENT Development
)
# install rule for the calendar data. we consider this part of the Runtime
# component because it is required for some libraries to function at runtime
install(DIRECTORY static_data/calendars DESTINATION share/OA COMPONENT Runtime)

# CMake package configuration

include(CMakePackageConfigHelpers)

# CMake script install subdir
set(ODIN_CMAKE_INSTALL_DIR lib/cmake/${PROJECT_NAME})
# name of exported targets file
set(ODIN_TARGETS_FILE ${PROJECT_NAME}Targets.cmake)
# generate package config file
set(ODIN_CONFIG_FILE cmake/${PROJECT_NAME}Config.cmake)
# variables consumed:
#
#   PROJECT_NAME
#   ODIN_TARGETS_FILE
#
configure_package_config_file(
    ${ODIN_CONFIG_FILE}.in ${ODIN_CONFIG_FILE}
    INSTALL_DESTINATION ${ODIN_CMAKE_INSTALL_DIR}
)
message(STATUS "Generated ${ODIN_CONFIG_FILE}")
# generate package version file
set(ODIN_CONFIG_VERSION_FILE cmake/${PROJECT_NAME}ConfigVersion.cmake)
write_basic_package_version_file(
    ${ODIN_CONFIG_VERSION_FILE}
    VERSION ${ODIN_VERSION}
    # TODO: no real compatibility guarantees so just use SameMajorVersion
    COMPATIBILITY SameMajorVersion
)
message(STATUS "Generated ${ODIN_CONFIG_VERSION_FILE}")
# install rule for config files
install(
    FILES
        ${PROJECT_BINARY_DIR}/${ODIN_CONFIG_FILE}
        ${PROJECT_BINARY_DIR}/${ODIN_CONFIG_VERSION_FILE}
    DESTINATION ${ODIN_CMAKE_INSTALL_DIR}
    COMPONENT Development
)
# install rule for exported targets
install(
    EXPORT odin_exports
    DESTINATION ${ODIN_CMAKE_INSTALL_DIR}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${ODIN_TARGETS_FILE}
    COMPONENT Development
)
