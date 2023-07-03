cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# set compile definitions
if(BUILD_SHARED_LIBS)
    message(STATUS "Building ${ODIN_PROJECT_NAME} dynamic libraries")
    # TODO: remove if we are always going to build as dynamic
    if(MSVC)
        add_compile_definitions(ODIN_DLL)
    endif()
endif()
# set compiler-specific definitions and options
if(MSVC)
    # disable warnings about insecure functions, C code is naturally unsafe
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(
        # preferred warning level + enable parallel compilaton. the CMake
        # -j, --parallel flag only builds *projects* in parallel
        /Wall /MP
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
        # /Od applied by default when using Debug configuration. this generates
        # debug info for all configs that are not Release config
        $<$<NOT:$<CONFIG:Release>>:/DEBUG>
    )
else()
    add_compile_options(
        -Wall
        # -O0 is default optimization level anyways, Clang ignores -ggdb
        $<$<NOT:$<CONFIG:Release>>:-ggdb> $<IF:$<CONFIG:Release>,-O3,-O0>
    )
endif()
