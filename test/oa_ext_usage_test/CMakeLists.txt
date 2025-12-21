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
add_test(NAME oa_print_dirs COMMAND oa_print_dirs)
