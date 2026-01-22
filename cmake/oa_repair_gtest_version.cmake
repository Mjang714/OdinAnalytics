cmake_minimum_required(VERSION 3.23)

include_guard(GLOBAL)

##
# oa_repair_gtest_version.cmake
#
# Provides a function to repair the value of GTest_VERSION if not defined.
#
# Unlike GTestConfig.cmake, the CMake-provided FindGTest.cmake find module does
# not define GTest_VERSION. This means that using the find module on versions
# of GoogleTest that are not built using CMake will result in GTest_VERSION not
# being defined. This module provides the eponymous CMake function to roughly
# perform this task; it is rough because GoogleTest headers provide *no*
# definable version information. E.g. we can only check if TYPED_TEST_SUITE()
# is defined to roughly bifurcate 1.8.x and 1.10.0 or higher.
#

include(CheckCXXSymbolExists)
include(CheckSourceCompiles)

##
# Repairs the value of GTest_VERSION if the variable is not defined.
#
# If GTest_VERSION already has a value then nothing is done. However, if empty
# or not defined, this function will first try to use pkg-config to locate an
# exact GoogleTest release version. If pkg-config is not available, it will
# fall back to using check_cxx_symbol_exists() to see if an identifying symbol,
# e.g. TYPED_TEST_SUITE(), is defined if gtest/gtest.h is included. E.g. if
# TYPED_TEST_SUITE() is defined, then we are using a version of GoogleTest that
# is 1.10.0 or higher. Otherwise, we simply assume that GoogleTest is version
# 1.8.1, the earliest possible GitHub release.
#
# For more detailed version checks we would need to follow the changes in
# https://github.com/google/googletest/releases to tell the differences.
#
# This function obtains the GoogleTest include directory using the properties
# of the GTest::gtest and/or GTest::gmock IMPORTED targets.
#
function(oa_repair_gtest_version)
    # no GTest::gtest which is an error
    if(NOT TARGET GTest::gtest)
        message(
            FATAL_ERROR
            "{CMAKE_CURRENT_FUNCTION}() cannot be invoked if GoogleTest has "
"not been found by CMake"
        )
    endif()
    # already defined so nothing to do
    if(GTest_VERSION)
        return()
    endif()
    # warn and proceed
    message(
        WARNING
        "GTest_VERSION is not defined by FindGTest.cmake and will be set by "
"${CMAKE_CURRENT_FUNCTION}() to an approximate value based on heuristics"
    )
    # first attempt to use pkg-config if possible
    if(NOT PkgConfig_FOUND)
        find_package(PkgConfig QUIET)
    endif()
    # if pkg-config found then look for GoogleTest
    if(PkgConfig_FOUND)
        pkg_search_module(gtest_pkg gtest)
        # if found set GTest_VERSION and return
        if(gtest_pkg_FOUND)
            set(GTest_VERSION ${gtest_pkg_VERSION} PARENT_SCOPE)
            return()
        endif()
    endif()
    # otherwise we use check_cxx_symbol_exists() on GoogleTest headers
    # get INTERFACE_INCLUDE_DIRECTORIES from target
    get_target_property(
        gtest_include_dirs GTest::gtest
        INTERFACE_INCLUDE_DIRECTORIES
    )
    # set include directories + libraries for check_cxx_symbol_exists()
    set(CMAKE_REQUIRED_INCLUDES ${gtest_include_dirs})
    set(CMAKE_REQUIRED_LIBRARIES GTest::gtest)
    # add GoogleMock target if available
    if(TARGET GTest::gmock)
        list(APPEND CMAKE_REQUIRED_LIBRARIES GTest::gmock)
    endif()
    # assume 1.8.1 since this is the earliest GitHub release
    set(GTest_VERSION 1.8.1 PARENT_SCOPE)
    # 1.10.0 check
    # unset gtest_check_pass (re-used multiple times so we want it fresh)
    unset(gtest_check_pass CACHE)
    # check for TYPED_TEST_SUITE() which is introduced in 1.10.0
    check_cxx_symbol_exists(TYPED_TEST_SUITE gtest/gtest.h gtest_check_pass)
    if(gtest_check_pass)
        set(GTest_VERSION 1.10.0 PARENT_SCOPE)
    endif()
    # 1.11.0 check
    unset(gtest_check_pass CACHE)
    # check for testing::IsNan() which is introduced in 1.11.0
    # note: GoogleTest and GoogleMock were merged. although they are still
    # separately distributed on some *nix platforms, pkg-config is usually
    # available on *nix platforms and we never need these checks
    check_cxx_symbol_exists(testing::IsNan gmock/gmock.h gtest_check_pass)
    if(gtest_check_pass)
        set(GTest_VERSION 1.11.0 PARENT_SCOPE)
    endif()
    # 1.12.0 check
    # note: no way to definitively check for 1.12.1
    # note: like other checks do this fresh
    unset(GTEST_AT_LEAST_1_12_0 CACHE)
    # check for testing::WhenBase64Unescaped which is introduced in 1.12.0.
    # since check_cxx_symbol_exists() doesn't work for templates we compile
    # source. we avoid gmock_main by using explicit init.
    check_source_compiles(
        CXX
        [[
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(Foo, Bar)
{
  EXPECT_THAT("A", ::testing::WhenBase64Unescaped("0"));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
        ]]
        GTEST_AT_LEAST_1_12_0
    )
    if(GTEST_AT_LEAST_1_12_0)
        set(GTest_VERSION 1.12.0 PARENT_SCOPE)
    endif()
    # 1.13.0 check
    unset(gtest_check_pass CACHE)
    # check for testing::SrcDir which is introduced in 1.13.0
    check_cxx_symbol_exists(testing::SrcDir gtest/gtest.h gtest_check_pass)
    if(gtest_check_pass)
        set(GTest_VERSION 1.13.0 PARENT_SCOPE)
    endif()

    # note: hard to determine from just the headers 1.14.0 through 1.16.0.
    # see https://github.com/google/googletest/releases

    # 1.17.0 check
    unset(GTEST_AT_LEAST_1_17_0 CACHE)
    # check for testing::DistanceFrom which is introduced in 1.17.0. this is a
    # template so again we cannot use check_cxx_symbol_exists()
    check_source_compiles(
        CXX
        [[
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(Foo, Bar)
{
  EXPECT_THAT(1.5, ::testing::DistanceFrom(1.6, ::testing::Lt(0.2)));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
        ]]
        GTEST_AT_LEAST_1_17_0
    )
    if(GTEST_AT_LEAST_1_17_0)
        set(GTest_VERSION 1.17.0 PARENT_SCOPE)
    endif()

    # note: add more GoogleTest version checks in *ascending* order here. the
    # idea is to test starting from oldest and ascend to be comprehensive. this
    # helps cope with the potential for a check criteria to be removed in a
    # later version of GoogleTest, e.g. a macro we expect is no longer there.

    # end with final version
endfunction()
