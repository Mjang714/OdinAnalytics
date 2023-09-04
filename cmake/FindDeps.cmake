cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# find magic_enum. can set ODIN_MAGIC_ENUM_ROOT in environment or define
if(DEFINED ENV{ODIN_MAGIC_ENUM_ROOT})
    message(
        STATUS
        "Using ODIN_MAGIC_ENUM_ROOT $ENV{ODIN_MAGIC_ENUM_ROOT} from env"
    )
    set(ODIN_MAGIC_ENUM_ROOT $ENV{ODIN_MAGIC_ENUM_ROOT})
endif()
# must be defined
if(NOT DEFINED ODIN_MAGIC_ENUM_ROOT)
    message(FATAL_ERROR "ODIN_MAGIC_ENUM_ROOT not defined")
endif()
# otherwise, add to include directories (SYSTEM ignores warnings)
include_directories(SYSTEM ${ODIN_MAGIC_ENUM_ROOT}/include)

# minimum Boost version
set(ODIN_BOOST_VERSION 1.71)
# find Boost. can set ODIN_BOOST_ROOT in environment or define, but we first
# attempt to find it in case there is an existing system installation
find_package(Boost ${ODIN_BOOST_VERSION})
# if not found, rely on environment variable
if(NOT Boost_FOUND)
    if(DEFINED ENV{ODIN_BOOST_ROOT})
        message(STATUS "Using ODIN_BOOST_ROOT $ENV{ODIN_BOOST_ROOT} from env")
        set(ODIN_BOOST_ROOT $ENV{ODIN_BOOST_ROOT})
    endif()
    # must be defined unless BOOST_ROOT already defined
    if(DEFINED ODIN_BOOST_ROOT)
        message(
            STATUS
            "Using BOOST_ROOT from ODIN_BOOST_ROOT ${ODIN_BOOST_ROOT}"
        )
        # official hint variable to set
        set(BOOST_ROOT ${ODIN_BOOST_ROOT})
    elseif(DEFINED BOOST_ROOT)
        message(STATUS "BOOST_ROOT already defined as ${BOOST_ROOT}")
    else()
        message(FATAL_ERROR "ODIN_BOOST_ROOT, BOOST_ROOT not defined")
    endif()
    # now go find Boost
    find_package(Boost ${ODIN_BOOST_VERSION} REQUIRED)
endif()
# add to includes
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# minimum Google Test version
set(ODIN_GTEST_VERSION 1.8)
# find Google Test. can set ODIN_GTEST_ROOT in environment or define, but we
# first attempt to find it in case there is an existing system installation
find_package(GTest ${ODIN_GTEST_VERSION})
# if not found, rely on environment variable
if(NOT GTest_FOUND)
    if(DEFINED ENV{ODIN_GTEST_ROOT})
        message(STATUS "Using ODIN_GTEST_ROOT $ENV{ODIN_GTEST_ROOT} from env")
        set(ODIN_GTEST_ROOT $ENV{ODIN_GTEST_ROOT})
    endif()
    # must be defined unless GTEST_ROOT already defined
    if(DEFINED ODIN_GTEST_ROOT)
        message(
            STATUS
            "Using GTEST_ROOT from ODIN_GTEST_ROOT ${ODIN_GTEST_ROOT}"
        )
        # official hint variable to set
        set(GTEST_ROOT ${ODIN_GTEST_ROOT})
    elseif(DEFINED GTEST_ROOT)
        message(STATUS "GTEST_ROOT already defined as ${GTEST_ROOT}")
    else()
        message(FATAL_ERROR "ODIN_GTEST_ROOT, GTEST_ROOT not defined")
    endif()
    # actual Google Test find_package command
    find_package(GTest ${ODIN_GTEST_VERSION} REQUIRED)
endif()
# not really necessary, but by using SYSTEM we suppress external warnings
include_directories(SYSTEM ${GTest_INCLUDE_DIRS})

# need Excel SDK on Windows. can set ODIN_EXCELSDK_ROOT in environment or
# define. this needs to be installed separately on a development machine
if(WIN32)
    if(DEFINED ENV{ODIN_EXCELSDK_ROOT})
        message(
            STATUS "Using ODIN_EXCELSDK_ROOT $ENV{ODIN_EXCELSDK_ROOT} from env"
        )
        set(ODIN_EXCELSDK_ROOT $ENV{ODIN_EXCELSDK_ROOT})
    endif()
    # must be defined
    if(NOT DEFINED ODIN_EXCELSDK_ROOT)
        message(FATAL_ERROR "ODIN_EXCELSDK_ROOT not defined")
    endif()
    # include, library, sample directories for the Excel SDK
    set(ODIN_EXCELSDK_INCLUDE_DIR ${ODIN_EXCELSDK_ROOT}/INCLUDE)
    set(ODIN_EXCELSDK_LIBRARY_DIR ${ODIN_EXCELSDK_ROOT}/LIB)
    set(ODIN_EXCELSDK_SAMPLE_DIR ${ODIN_EXCELSDK_ROOT}/SAMPLES)
    # include, library directories for frmwrk32.lib sample
    set(ODIN_EXCELSDK_FRAMEWRK_DIR ${ODIN_EXCELSDK_SAMPLE_DIR}/FRAMEWRK)
    set(
        ODIN_EXCELSDK_FRAMEWRK_LIBRARY_DIR
        ${ODIN_EXCELSDK_FRAMEWRK_DIR}/$<IF:$<CONFIG:Debug>,DEBUG,RELEASE>
    )
    # check that xlcall32.lib exists in the library directory (error if failed)
    find_library(XLCALL32 xlcall32 HINTS ${ODIN_EXCELSDK_LIBRARY_DIR} NO_CACHE)
    if(XLCALL32 STREQUAL XLCALL32-NOTFOUND)
        message(
            FATAL_ERROR
            "Failed to locate xlcall32.lib with ODIN_EXCELSDK_ROOT \
${ODIN_EXCELSDK_ROOT}"
        )
    endif()
    # on success, update include and library dirs
    include_directories(SYSTEM ${ODIN_EXCELSDK_INCLUDE_DIR})
    link_directories(${ODIN_EXCELSDK_LIBRARY_DIR})
    # we are using frmwrk32.lib which needs to be build manually using the
    # provided BUILD.bat and MAKE.bat scripts, so look for that too. we need
    # to use debug and release versions appropriately, since frmwrk32 is linked
    # against debug + release dynamic CRT appropriately.
    find_library(
        FRMWRK32 frmwrk32
        HINTS
        # TODO: if one of them is missing but the other isn't, the issue will
        # not be noticed until link time. i think find_library doesn't accept
        # generator expressions, so that's an issue
        ${ODIN_EXCELSDK_FRAMEWRK_DIR}/DEBUG
        ${ODIN_EXCELSDK_FRAMRWRK_DIR}/RELEASE
        NO_CACHE
    )
    if(FRMWRK32 STREQUAL FRMWRK32-NOTFOUND)
        message(
            FATAL_ERROR
            "Failed to locate frmwrk32.lib with ODIN_EXCELSDK_ROOT \
${ODIN_EXCELSDK_ROOT}. \
Please cd to ${ODIN_EXCELSDK_SAMPLE_DIR} and run
make DEBUG BUILD && make RELEASE BUILD
"
        )
    endif()
    # on success, update include and library dirs
    include_directories(SYSTEM ${ODIN_EXCELSDK_FRAMEWRK_DIR})
    link_directories(${ODIN_EXCELSDK_FRAMEWRK_LIBRARY_DIR})
endif()
