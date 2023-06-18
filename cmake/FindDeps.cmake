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
include_directories(SYSTEM ${ODIN_MAGIC_ENUM_ROOT})

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
        # official hint variable to set
        set(BOOST_ROOT ${ODIN_BOOST_ROOT})
    endif()
    # must be defined unless BOOST_ROOT already defined
    if(DEFINED ODIN_BOOST_ROOT)
        message(
            STATUS
            "Using BOOST_ROOT from ODIN_BOOST_ROOT ${ODIN_BOOST_ROOT}"
        )
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
        # official hint variable to set
        set(GTEST_ROOT ${ODIN_GTEST_ROOT})
    endif()
    # must be defined unless GTEST_ROOT already defined
    if(DEFINED ODIN_GTEST_ROOT)
        message(STATUS "GTEST_ROOT already defined as ${GTEST_ROOT}")
    else()
        message(FATAL_ERROR "ODIN_GTEST_ROOT, GTEST_ROOT not defined")
    endif()
    # actual Google Test find_package command
    find_package(GTest ${ODIN_GTEST_VERSION} REQUIRED)
endif()
# not really necessary, but by using SYSTEM we suppress external warnings
include_directories(SYSTEM ${GTest_INCLUDE_DIRS})
