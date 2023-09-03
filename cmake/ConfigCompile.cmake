cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# build shared libraries by default
option(BUILD_SHARED_LIBS "Build shared libraries." ON)

# set compile definitions
if(BUILD_SHARED_LIBS)
    message(STATUS "Building ${ODIN_PROJECT_NAME} dynamic libraries")
    # indicate that Odin libraries are all being used as dynamic libraries
    add_compile_definitions(OA_DLL)
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
