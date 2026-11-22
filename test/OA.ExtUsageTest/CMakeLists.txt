cmake_minimum_required(VERSION 3.20)

# OA external usage test project
project(
    OAExtUsageTest
    VERSION 0.1.0
    DESCRIPTION "OA external usage test"
    LANGUAGES CXX
)

# C++ standard is set by CMake config call via CMAKE_CXX_STANDARD
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# note: OA_ROOT required for this test
if(NOT DEFINED OA_ROOT)
    message(FATAL_ERROR "OA_ROOT required for oa_print_dirs test")
endif()

# look for OA using OA_TARGET_VERSION provided by the CMake config call
find_package(OA ${OA_TARGET_VERSION} REQUIRED)
message(STATUS "OA version: ${OA_VERSION}")

include(CTest)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall)
endif()

# oa_print_dirs: print some OA directories using oa_config functions
add_executable(oa_print_dirs oa_print_dirs.cpp)
target_link_libraries(oa_print_dirs PRIVATE OA::oa_config)

# smoke test for oa_print_dirs
# note: on Windows ensure that the installed DLLs are locatable by using cmake
# -E env as the semicolons in PATH make it hard to use set_tests_proprties().
# we don't copy the dependent DLLs to the oa_print_dirs directory because that
# results in the oa::config:: path functions being loaded from the copied DLLs
# and thus returning values that correspond to a non-installed distribution.
if(WIN32)
    add_test(
        NAME oa_print_dirs
        # note: quote entire PATH=<value> to ensure it is passed as one string
        COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH};${OA_ROOT}/bin" --
                $<TARGET_FILE:oa_print_dirs>
    )
else()
    add_test(NAME oa_print_dirs COMMAND oa_print_dirs)
endif()
# ensure test fails if oa::config::install_dir() returns empty path
set_tests_properties(
    oa_print_dirs PROPERTIES
    FAIL_REGULAR_EXPRESSION "OA rootdir: \"\""
)
