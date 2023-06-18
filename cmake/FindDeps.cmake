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

# find Boost. can set ODIN_BOOST_ROOT in environment or define
if(DEFINED ENV{ODIN_BOOST_ROOT})
    message(STATUS "Using ODIN_BOOST_ROOT $ENV{ODIN_BOOST_ROOT} from env")
    set(ODIN_BOOST_ROOT $ENV{ODIN_BOOST_ROOT})
    # official hint variable to set
    set(BOOST_ROOT ${ODIN_BOOST_ROOT})
endif()
# must be defined unless BOOST_ROOT already defined
if(DEFINED ODIN_BOOST_ROOT)
    message(STATUS "Using BOOST_ROOT from ODIN_BOOST_ROOT ${ODIN_BOOST_ROOT}")
    set(BOOST_ROOT ${ODIN_BOOST_ROOT})
elseif(DEFINED BOOST_ROOT)
    message(STATUS "BOOST_ROOT already defined as ${BOOST_ROOT}")
else()
    message(FATAL_ERROR "ODIN_BOOST_ROOT, BOOST_ROOT not defined")
endif()
# actual Boost find_package command + add to includes
find_package(Boost 1.79 REQUIRED COMPONENTS)
include_directories(SYSTEM ${ODIN_BOOST_ROOT})
