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

include(oa_embed_version_info)
include(oa_pdb_install)
include(oa_r_discover_tests)
include(oa_r_package)
include(oa_repair_gtest_version)
include(oa_swig_module)
include(oa_unittest_discover_tests)
include(oa_unity_enable_unique_id)

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
# note: bumped from the ancient 1.8 as <1.11.0 uses std::result_of in
# gmock-actions.h which was removed in C++20
find_package(GTest 1.11.0)
if(GTest_FOUND)
    # set GTest_VERSION approximately if GTest found using FindGTest.cmake
    # note: if GTest_VERSION already set nothing is done
    oa_repair_gtest_version()
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

# find Google's benchmarking library
# note: GetBenchmarkVersion() was added in the 1.8.4 release to allow runtime
# querying of the benchmark library version being used
find_package(benchmark 1.6.1)
if(benchmark_FOUND)
    message(STATUS "Benchmark version: ${benchmark_VERSION}")
else()
    message(STATUS "Benchmark version: None")
endif()

# find OpenBLAS
#
# note: this is the preferred [C]BLAS/LAPACK[E] implementation due to the
# permissive licensing + portability compared to e.g. Intel MKL
#
# note: up through version 0.3.34, OpenBLAS's lapack.h inappropriately uses
# function prototypes returning lapack_int instead of void. this causes havoc
# when mixed with libraries like Armadillo that declare the correct prototypes
# that have void as the return type. fortunately, this issue is fixed in the
# OpenBLAS trunk as of commit bbac7dfb82100b33af1f1390ac3c14e50ad8b65f
#
find_package(OpenBLAS 0.3.33)
if(OpenBLAS_FOUND)
    message(STATUS "OpenBLAS version: ${OpenBLAS_VERSION}")
    # print config for more details
    # library type, either static *or* shared
    # note: OpenBLAS_(static|shared)_FOUND only TRUE if requested + found
    get_target_property(openblas_libtype OpenBLAS::OpenBLAS TYPE)
    if(openblas_libtype STREQUAL "STATIC_LIBRARY")
        message(STATUS "OpenBLAS static: Yes")
        set(OA_OPENBLAS_STATIC TRUE)
    elseif(openblas_libtype STREQUAL "SHARED_LIBRARY")
        message(STATUS "OpenBLAS static: No")
        set(OA_OPENBLAS_STATIC FALSE)
    endif()
    unset(openblas_libtype)
    # threading
    if(OpenBLAS_serial_FOUND)
        message(STATUS "OpenBLAS threads: None")
    # note: generic build on Windows has pthread component
    elseif(OpenBLAS_pthread_FOUND)
        if(WIN32)
            message(STATUS "OpenBLAS threads: Win32")
        else()
            message(STATUS "OpenBLAS threads: POSIX")
        endif()
    elseif(OpenBLAS_openmp_FOUND)
        message(STATUS "OpenBLAS threads: OpenMP")
    endif()
    # print all OpenBLAS CPU features from the config header
    # note: OpenBLAS has as bug where OpenBLAS_INCLUDE_DIR is empty because it
    # uses the OpenBLAS::OpenBLAS INCLUDE_DIRECTORIES property, not the
    # INTERFACE_INCLUDE_DIRECTORIES property that it should be using
    # note: empty for generic build on Windows
    file(
        STRINGS "${OpenBLAS_INCLUDE_DIRS}/openblas_config.h" openblas_haves
        REGEX "#define[ \t]+OPENBLAS_HAVE_[A-Z0-9_]+"
    )
    # strip #define OPENBLAS_HAVE_ prefixes + join with spaces
    list(TRANSFORM openblas_haves REPLACE "#define[ \t]+OPENBLAS_HAVE_" "")
    list(JOIN openblas_haves " " openblas_haves)
    # print + clean up
    message(STATUS "OpenBLAS features: ${openblas_haves}")
    unset(openblas_haves)
    # add interface target to encapsulate OpenBLAS usage requirements
    add_library(OA::OpenBLAS INTERFACE IMPORTED)
    target_link_libraries(OA::OpenBLAS INTERFACE OpenBLAS::OpenBLAS)
    # when static and on Linux we may need to link against libgfortran
    # TODO: by default this is a dynamic link + libquadmath is also dynamically
    # linked. may need to consider how to static link these two
    if(OA_OPENBLAS_STATIC AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(OA::OpenBLAS INTERFACE gfortran)
    endif()
    #
    # for MSVC, in C++ mode, the C99 _Complex is not available, and _FComplex
    # and _DComplex were removed in Windows 10 SDK from <complex.h> unless one
    # defines _CRT_USE_C_COMPLEX_H. in C++, we typically want std::complex for
    # native C++ functionality, so we use LAPACK_COMPLEX_CUSTOM for this.
    #
    # TODO: consider just defining HAVE_LAPACK_CONFIG_H to enable inclusion of
    # lapacke_config.h to provide some cross-platform functionality
    #
    if(MSVC)
        target_compile_definitions(
            OA::OpenBLAS INTERFACE
            # indicate using custom complex types with LAPACK
            LAPACK_COMPLEX_CUSTOM
            # note: for strict CBLAS compatibility these could be void, but
            # that causes problems with some LAPACKE functions that take these
            # types by value, as void is an incomplete type
            lapack_complex_float=std::complex<float>
            lapack_complex_double=std::complex<double>
            # define lapack_complex_(float|double)_(real|imag) macros since
            # these are lost when defining LAPACK_COMPLEX_CUSTOM
            "lapack_complex_float_real(z)=(z).real()"
            "lapack_complex_float_imag(z)=(z).imag()"
            "lapack_complex_double_real(z)=(z).real()"
            "lapack_complex_double_imag(z)=(z).imag()"
            #
            # FIXME:
            #
            # defining __EMSCRIPTEN__ disables the lapack.h definition of
            # LAPACK_FORTRAN_STRLEN_END, i.e. using the LAPACK function
            # prototypes that don't have the "hidden" length arguments at the
            # end of the functions with const char* arguments. this is required
            # for correct interop with Armadillo incorrectly const-qualifies
            # the cgeevx, zgeevx info params in armadillo_bits/def_lapack.hpp,
            # but only for the signatures where ARMA_USE_FORTRAN_HIDDEN_ARGS is
            # defined, i.e. the signatures without the "hidden" length args.
            #
            # it is, of course, a hack to pretend that we are the Emscripten
            # compiler when we are using MSVC, so this bug needs to be reported
            # to Armadillo and fixed for the next 15.6.x release.
            #
            __EMSCRIPTEN__
        )
        # force include <complex> since LAPACK_COMPLEX_CUSTOM gets rid of the
        # <complex> includes when compiling in C++ mode
        # note: /FI actually includes <complex> as "complex"
        target_compile_options(OA::OpenBLAS INTERFACE /FIcomplex)
    endif()
    # FIXME: until OpenBLAS has a new tagged version that fixes the lapack.h
    # functions returning int instead of void we ensure that our vendored copy
    # of the fixed lapack.h is force-included to prevent including the broken
    # lapack.h provided by OpenBLAS 0.3.22 through 0.3.34. we accomplish this
    # by using an absolute path for the include to the vendored lapack.h
    set(
        openblas_vendor_root
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/OpenBLAS-bbac7df"
    )
    set(
        openblas_lapack_vendor_include
        "${openblas_vendor_root}/lapack-netlib/LAPACKE/include"
    )
    if(MSVC)
        target_compile_options(
            OA::OpenBLAS INTERFACE
            # note: must force-include *after* <complex> is force-included
            # since we are using std::complex for lapack_complex_(float|double)
            /FI"${openblas_lapack_vendor_include}/lapack.h"
        )
    # note: assumes GNU-like compiler
    else()
        target_compile_options(
            OA::OpenBLAS INTERFACE
            -include "${openblas_lapack_vendor_include}/lapack.h"
        )
    endif()
else()
    message(STATUS "OpenBLAS version: None")
endif()

# find Eigen 3 and above
#
# note: for correct LAPACK function prototypes we need Eigen 5.0 or above.
# previous versions had LAPACK functions return int instead of void which is
# inconsistent with the expected translations of Fortran subroutines to C
#
find_package(Eigen3 5.0)
if(Eigen3_FOUND)
    message(STATUS "Eigen3 version: ${Eigen3_VERSION}")
    # add interface library to collect our own usage requirements
    add_library(OA::Eigen3 INTERFACE IMPORTED)
    target_link_libraries(OA::Eigen3 INTERFACE Eigen3::Eigen)
    # prefer to use /bigobj with MSVC due to template code bloat
    # TODO: maybe enable globally for the project or just for ols_demo
    if(MSVC)
        target_compile_options(OA::Eigen3 INTERFACE /bigobj)
    endif()
    # if OpenBLAS found use it as the LAPACKE implementation provider
    #
    # note: in Eigen/src/misc/blas.h for Eigen < 5.0.0, BLASLONG is a typedef
    # of long long only when __WIN64__ is defined. MSVC no longer seems to
    # define this when compiling in 64-bit mode (_WIN64 is the correct macro)
    # so we have to manually define __WIN64__ when compiling with MSVC.
    # fortunately, Eigen 5.0.0 removes this problematic typedef so we don't
    # need to explicitly define __WIN64__ ourselves during compilation
    #
    message(STATUS "Eigen3 BLAS: Eigen3")
    if(OpenBLAS_FOUND)
        message(STATUS "Eigen3 LAPACKE: OpenBLAS")
        # update usage requirements
        target_compile_definitions(
            OA::Eigen3 INTERFACE
            #
            # FIXME:
            #
            # Eigen/src/misc/blas.h has incorrect prototypes for complex-valued
            # BLAS functions like cherk as all complex-valued types are reduced
            # to their real-valued types. also, unlike with Armadillo, we don't
            # have any backdoor macros to use to redefine the complex types, so
            # it's best to just avoid EIGEN_USE_BLAS usage. in either way,
            # Eigen's own performance is comparable, so not big loss.
            #
            # interestingly, we do not have this issue with EIGEN_USE_LAPACKE,
            # as Eigen/src/misc/lapacke.h is the Intel LAPACKE header which has
            # correct prototypes, but doesn't support the "hidden" arguments
            # like the actual LAPACKE lapacke.h header does. however, due to
            # the previously mentioned bug in Armadillo's def_lapack.hpp this
            # works out in our favor since we are defining __EMSCRIPTEN__.
            #
            EIGEN_USE_LAPACKE
        )
        # note: ensure we use OA::OpenBLAS to consume any changes made to
        # OpenBLAS usage requirements, e.g. for MSVC the new complex defines
        target_link_libraries(OA::Eigen3 INTERFACE OA::OpenBLAS)
    # otherwise Eigen should fall back on its own implementations
    else()
        message(STATUS "Eigen3 LAPACKE: Eigen3")
    endif()
else()
    message(STATUS "Eigen3 version: None")
endif()

# find Armadillo
# note: this uses our custom find module to work around CMake's FindArmadillo
# and the actual Armadillo project's CMake config script
find_package(Armadillo 15.6)
if(Armadillo_FOUND)
    message(STATUS "Armadillo version: ${Armadillo_VERSION}")
    # we create our own INTERFACE target for better control
    add_library(OA::Armadillo INTERFACE IMPORTED)
    target_link_libraries(OA::Armadillo INTERFACE Armadillo::Armadillo)
    # if OpenBLAS found add to the link interface
    if(OpenBLAS_FOUND)
        message(STATUS "Armadillo BLAS: OpenBLAS")
        message(STATUS "Armadillo LAPACK: OpenBLAS")
        # strictly speaking the Armadillo def_lapack.hpp and def_blas.hpp
        # headers define blas_cxf and blas_cxd as void to adhere to CBLAS
        # conventions. fortunately, we can use ARMA_USE_MKL_TYPES to enable
        # defining complex types similar to using LAPACK_COMPLEX_CUSTOM
        target_compile_definitions(OA::Armadillo INTERFACE ARMA_USE_MKL_TYPES)
        # for MSVC use std::complex and also use the function prototypes that
        # don't have the "hidden" length arguments because cgeevz, zgeevx in
        # def_lapack.hpp have info incorrectly const-qualified. see above for
        # why __EMSCRIPTEN__ is defined on Windows for OpenBLAS usage
        if(MSVC)
            target_compile_definitions(
                OA::Armadillo INTERFACE
                MKL_Complex8=std::complex<float>
                MKL_Complex16=std::complex<double>
                ARMA_DONT_USE_FORTRAN_HIDDEN_ARGS
            )
        # for GNU-like compilers use the _Complex extension
        else()
            target_compile_definitions(
                OA::Armadillo INTERFACE
                "MKL_Complex8=float _Complex"
                "MKL_Complex16=double _Complex"
            )
        endif()
        # propagate any OpenBLAS usage requirements we defined
        target_link_libraries(OA::Armadillo INTERFACE OA::OpenBLAS)
    # note: no other LAPACK alternative yet
    else()
        message(STATUS "Armadillo BLAS: None")
        message(STATUS "Armadillo LAPACK: None")
    endif()
else()
    message(STATUS "Armadillo version: None")
endif()

# look for Excel 2013 XLL SDK
# note: we don't actually need the GENERIC sample XLL but it is a nice example
if(WIN32)
    find_package(XLLSDK 15.0 COMPONENTS Framework Generic)
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

# look for Python 3 interpreter + development headers/libraries
# note: 32-bit Python is almost never used now
# note: to force FindPython3 to only look in venv set Python3_FIND_VIRTUALENV
# to ONLY. by default, FindPython3 will still look outside a venv for Python
find_package(Python3 3.10 COMPONENTS Interpreter Development)
# FindPython3 does an architecture check when Interpreter and Development are
# both specified. however, since oxlgen.py is agnostic to whether the Python
# used is 32- or 64-bit, this is overly restrictive; we can build a 32-bit
# Excel add-in with a 64-bit Python. so this time we only look for interpreter
if(NOT Python3_FOUND)
    find_package(Python3 3.10 COMPONENTS Interpreter)
endif()
# report
if(Python3_FOUND)
    # compute Python architecture. sys.maxsize is the max value of the signed
    # Py_ssize_t so we need to count an extra bit and take log of maxsize + 1
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" -c
                "import math; import sys; print(1 + int(math.log2(sys.maxsize + 1)))"
        OUTPUT_VARIABLE ODIN_PYTHON_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    # print messages
    message(STATUS "Python version: ${Python3_VERSION}")
    message(STATUS "Python architecture: ${ODIN_PYTHON_ARCH}")
    if(Python3_Development_FOUND)
        message(STATUS "Python libraries: Yes")
    else()
        message(STATUS "Python libraries: No")
    endif()
else()
    message(STATUS "Python version: None")
endif()

# find the R executables + development artifacts
find_package(R 4.1 COMPONENTS Interpreter OPTIONAL_COMPONENTS Development)
if(R_FOUND)
    message(STATUS "R version: ${R_VERSION}")
    # architecture
    if(R_64BIT)
        message(STATUS "R architecture: 64")
    else()
        message(STATUS "R architecture: 32")
    endif()
    # library
    if(TARGET R::libR)
        message(STATUS "R libraries: Yes")
    else()
        message(STATUS "R libraries: No")
    endif()
else()
    message(STATUS "R version: None")
endif()

# look for SWIG
# note: we don't use UseSWIG since swig_add_library() fails to correctly re-run
# incremental builds with Makefile generators. see oa_swig_module.cmake
find_package(SWIG 4.0 OPTIONAL_COMPONENTS python r)
if(SWIG_FOUND)
    message(STATUS "SWIG version: ${SWIG_VERSION}")
else()
    message(STATUS "SWIG version: None")
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
        # C4668: macro not defined and replaced with 0. this is turned on with
        # /Wall, but since we are using /W4, we explicitly enable
        /wd4668
        # specific /Wall warnings we are ok with disabling
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
        # note: /Od applied by default when using Debug configuration
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
        # ensure we warn if a macro is used without being first defined
        -Wundef
        # -O0 is default optimization level anyways, Clang ignores -ggdb
        $<$<NOT:$<CONFIG:Release>>:-ggdb> $<IF:$<CONFIG:Release>,-O3,-O0>
    )
endif()

# SWIG-specific default compile options
if(SWIG_FOUND)
    oa_swig_compile_options(-Wall)
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
# set Python version string (use "+" instead for local versions)
string(REPLACE "-" "+" ODIN_PY_VERSION "${ODIN_VERSION}")

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
# TODO: go through project targets to see which implicitly depend on this. note
# that linkage could be PRIVATE or PUBLIC depending on header exposure
add_library(oa_core INTERFACE)
target_include_directories(
    oa_core INTERFACE
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    # note: build interface includes generated headers
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
# install rule for oa_core (provides just an IMPORTED target)
install(TARGETS oa_core EXPORT odin_exports)

add_subdirectory(sil)
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

# set variables for CPack
# note: although CPACK_PACKAGE_NAME defaults to PROJECT_NAME we want to
# explicitly reuse its value in CPACK_PACKAGE_VENDOR
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "${CPACK_PACKAGE_NAME} Developers")
set(CPACK_PACKAGE_VERSION "${ODIN_VERSION}")
# if running on GitHub Actions ODIN_GITHUB_ACTIONS_RUNNER and
# ODIN_GITHUB_ACTIONS_ARCH need to be set. these will be used to set
# CPACK_SYSTEM_NAME to influence CPACK_PACKAGE_FILE_NAME
if(DEFINED ENV{GITHUB_ACTIONS})
    if(NOT ODIN_GITHUB_ACTIONS_RUNNER)
        message(
            FATAL_ERROR
            "ODIN_GITHUB_ACTIONS_RUNNER should be defined to the GitHub Actions "
"runner image name, e.g. the value of jobs.<job_id>.runs-on"
        )
    endif()
    if(NOT ODIN_GITHUB_ACTIONS_ARCH)
        message(
            FATAL_ERROR
            "ODIN_GITHUB_ACTIONS_ARCH should be defined to the GitHub Actions "
"runner architecture, e.g. runner.arch, or the build architecture itself, e.g. "
"Win32, x64, etc."
        )
    endif()
    set(
        CPACK_SYSTEM_NAME
        "${ODIN_GITHUB_ACTIONS_RUNNER}-${ODIN_GITHUB_ACTIONS_ARCH}"
    )
endif()
set(CPACK_RESOURCE_FILE_LICENSE LICENSE)
set(CPACK_RESOURCE_FILE_README README.md)
# generate CPackConfig.cmake in build directory
include(CPack)
