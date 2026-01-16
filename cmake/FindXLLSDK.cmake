cmake_minimum_required(VERSION 3.21)

##
# FindXLLSDK.cmake
#
# Locate an installation of the Microsoft Excel 2013 XLL SDK.
#
# On success, the following variables are defined:
#
#   XLLSDK_VERSION          XLL SDK major.minor version in XLCALL.H
#   XLLSDK_INCLUDE_DIRS     XLL SDK include directories
#   XLLSDK_LIBRARIES        XLL SDK [static] libraries
#   XLLSDK_64BIT            TRUE if XLL SDK libraries are 64-bit
#
# The following CMake IMPORTED STATIC target is defined:
#
#   XLLSDK::SDK             Target for linking against xlcall32.lib
#
# Furthermore, using the SRC\XLCALL.CPP source file provided since the 2007
# Excel XLL SDK, the following CMake ALIAS static library target is provided:
#
#   XLLSDK::SDK12           Target for linking against xlcall32.lib and the
#                           Excel12() and Excel12v() definitions in XLCALL.CPP
#
# The SDK12 target is an alias to the XLCALL2012 static library target that
# will be added to the calling project's build tree for simpler code reuse.
#
# A typical Excel 2013 XLL SDK install will also provide a SAMPLES\FRAMEWRK
# directory for the frmwrk32 library. If this directory exists the following
# CMake ALIAS static library target will be defined if requested:
#
#   XLLSDK::Framework       Target for linking against frmwrk32.lib
#
# The Excel 2013 XLL SDK tree also contains a SAMPLES\GENERIC directory for the
# GENERIC sample XLL. If this directory exists the following CMake ALIAS module
# library target will be defined if requested:
#
#   XLLSDK::Generic         Target representing the generic.xll add-in
#
# Selection of 32-bit and 64-bit libraries is automatically done.
#
# Note that both the Framework and Generic targets will be aliases to actual
# targets added to the calling project's build tree. This is because in 2025
# the Microsoft-provided NMake Makefiles in the Excel2013XLLSDK\SAMPLES subdirs
# are out of date and won't build under newer Visual Studio compiler versions,
# e.g. 2022. The BUILD.BAT and MAKE.BAT have the additional issue of looking
# for vcvarsalls.bat in %VCINSTALLDIR% directly when this script has now been
# moved to %VCINSTALLDIR%\Auxiliary\Build instead.
#
# Example:
#
#   # require xlcall32.lib, XLCALL.CPP
#   find_package(XLLSDK 15.0 REQUIRED)
#
#   # require xlcall32.lib, XLCALL.CPP, frmwrk32.lib
#   find_package(XLLSDK 15.0 REQUIRED COMPONENTS Framework)
#
#   # require xlcall32.lib, XLCALL.CPP, frmwrk32.lib, generic.xll
#   find_package(XLLSDK 15.0 REQUIRED COMPONENTS Framework Generic)
#

include(FindPackageHandleStandardArgs)

# supported components
set(XLLSDK_SUPPORTED_COMPONENTS SDK Framework Generic)
# check that all specified components are valid
foreach(_comp ${XLLSDK_FIND_COMPONENTS})
    list(FIND XLLSDK_SUPPORTED_COMPONENTS ${_comp} _xllsdk_${_comp}_pos)
    if(_xllsdk_${_comp}_pos EQUAL -1)
        list(JOIN XLLSDK_SUPPORTED_COMPONENTS " " _xllsdk_components_str)
        message(
            FATAL_ERROR
            "${_comp} is not one of the following valid XLL SDK components: "
"${_xllsdk_components_str}"
        )
    endif()
    unset(_xllsdk_${_comp}_pos)
endforeach()

# locate the xlcall.h header
find_path(XLLSDK_INCLUDE_DIRS xlcall.h PATH_SUFFIXES include NO_CACHE)
# not found, so stop early
if(NOT XLLSDK_INCLUDE_DIRS)
    find_package_handle_standard_args(XLLSDK REQUIRED_VARS XLLSDK_INCLUDE_DIRS)
    return()
endif()

# parse major.minor version from header + sanity check
file(
    STRINGS "${XLLSDK_INCLUDE_DIRS}/xlcall.h" XLLSDK_VERSION
    REGEX "[ ]+Version[ ]+[0-9]+\\.[0-9]+"
)
if(NOT XLLSDK_VERSION)
    find_package_handle_standard_args(
        XLLSDK
        REQUIRED_VARS XLLSDK_VERSION
        REASON_FAILURE_MESSAGE "Unable to parse XLCALL.H version"
    )
    return()
endif()
# remove non-version components
string(
    REGEX REPLACE "^\\*\\*[ ]+Version[ ]+([0-9]+\\.[0-9]+)" "\\1"
    XLLSDK_VERSION "${XLLSDK_VERSION}"
)

# explicit version check so we can skip all the other steps
find_package_check_version(
    ${XLLSDK_VERSION} XLLSDK_FOUND
    HANDLE_VERSION_RANGE
    RESULT_MESSAGE_VARIABLE _xllsdk_version_err)
if(NOT XLLSDK_FOUND)
    message(STATUS "Could NOT find XLLSDK: ${_xllsdk_version_err}")
    unset(_xllsdk_version_err)
    return()
endif()

# 64-bit build target or not?
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(XLLSDK_64BIT TRUE)
else()
    set(XLLSDK_64BIT FALSE)
endif()

# xlcall32 library subdirectory changes if target is 64-bit
if(XLLSDK_64BIT)
    set(_xllsdk_libdir lib/x64)
else()
    set(_xllsdk_libdir lib)
endif()

# get root install directory
# note: had some issues with trailing / with cmake_path
set(_xllsdk_root "${XLLSDK_INCLUDE_DIRS}/..")

# locate xlcall32. search *only* the library root with NO_DEFAULT_PATH
find_library(
    XLLSDK_XLCALL32 xlcall32.lib
    HINTS "${_xllsdk_root}"
    PATH_SUFFIXES ${_xllsdk_libdir}
    NO_CACHE
    NO_DEFAULT_PATH
)
if(NOT XLLSDK_XLCALL32)
    find_package_handle_standard_args(XLLSDK REQUIRED_VARS XLLSDK_XLCALL32)
    return()
endif()

# define SDK IMPORTED target
add_library(XLLSDK::SDK STATIC IMPORTED)
target_include_directories(XLLSDK::SDK INTERFACE "${XLLSDK_INCLUDE_DIRS}")
set_target_properties(
    XLLSDK::SDK PROPERTIES
    IMPORTED_LOCATION "${XLLSDK_XLCALL32}"
)
# define XLCALL2012 target + SDK12 IMPORTED targets for Excel12() + Excel12v()
add_library(XLCALL2012 STATIC "${_xllsdk_root}/src/xlcall.cpp")
add_library(XLLSDK::SDK12 ALIAS XLCALL2012)
target_link_libraries(XLCALL2012 PUBLIC XLLSDK::SDK)
# never use unity build for XLCALL2012 to make it clear what is being built
set_target_properties(XLCALL2012 PROPERTIES UNITY_BUILD FALSE)
# mark as located + set XLLSDK_LIBRARIES
# note: usually never need to pass SDK to COMPONENTS
set(XLLSDK_SDK_FOUND TRUE)
set(XLLSDK_LIBRARIES "${XLLSDK_XLCALL32}" XLLSDK::SDK12)

# get framework library source directory
set(_xllsdk_frmwrk_srcdir "${_xllsdk_root}/samples/framewrk")

# directly build frmwrk32 as part of the calling project
# note: not defining _USRDLL during compilation. do we need to (for MFC)?
if(EXISTS "${_xllsdk_frmwrk_srcdir}")
    add_library(
        frmwrk32 STATIC
        "${_xllsdk_frmwrk_srcdir}/framewrk.c"
        "${_xllsdk_frmwrk_srcdir}/MemoryManager.cpp"
        "${_xllsdk_frmwrk_srcdir}/MemoryPool.cpp"
    )
    add_library(XLLSDK::Framework ALIAS frmwrk32)
    # expose _xllsdk_frmwrk_srcdir as part of include interface due to use of
    # <memorymanager.h> include + CMake won't be running in same directory
    # note: MemoryManager.h and MemoryPool.h are not specified in Microsoft
    # documentation as part of the frmwrk32 include interface but we can't
    # really stop a project from including these files
    target_include_directories(frmwrk32 PUBLIC "${_xllsdk_frmwrk_srcdir}")
    # need extra private include directories due to weird setup
    target_include_directories(
        frmwrk32 PRIVATE
        # need to be able to include xlcall.cpp and xlcall.h respectively
        "${_xllsdk_root}/src" "${XLLSDK_INCLUDE_DIRS}"
    )
    # explicitly need to compile FRAMEWRK.C as C code
    set_source_files_properties(
        "${_xllsdk_frmwrk_srcdir}/framewrk.c" PROPERTIES
        LANGUAGE C
    )
    set_target_properties(
        frmwrk32 PROPERTIES
        # when building the Debug config we want the frmwrk32 PDB to be in the
        # same output directory as frmwrk32.lib and be named frmwrk32.pdb
        COMPILE_PDB_NAME_DEBUG frmwrk32
        COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG $<TARGET_FILE_DIR:frmwrk32>
        # ensure we *never* use unity build when building frmwrk32
        UNITY_BUILD FALSE
    )
    # mark as found + add target to XLLSDK_LIBRARIES
    set(XLLSDK_Framework_FOUND TRUE)
    list(APPEND XLLSDK_LIBRARIES "XLLSDK::Framework")
endif()

# get generic library source directory
set(_xllsdk_generic_srcdir "${_xllsdk_root}/samples/generic")

# directly build generic XLL as part of the calling project
# note: not defining _USRDLL during compilation. do we need to (for MFC)?
if(EXISTS "${_xllsdk_generic_srcdir}")
    add_library(
        generic MODULE
        "${_xllsdk_generic_srcdir}/generic.c"
        # note: excluding generic.def because DLL exported functions already
        # have __declspec(dllexport) attributes
        "${_xllsdk_generic_srcdir}/generic.rc"
    )
    add_library(XLLSDK::Generic ALIAS generic)
    # suppress C4312 we cannot do anything about
    target_compile_options(generic PRIVATE /wd4312)
    # since _xllsdk_generic_srcdir is not part of the project's source tree we
    # need to add it to the list of include directories
    target_include_directories(generic PRIVATE "${_xllsdk_generic_srcdir}")
    # generic XLL uses both SDK and Framework library
    target_link_libraries(generic PRIVATE XLLSDK::SDK XLLSDK::Framework)
    # again, explicitly need to compile GENERIC.C as C code
    set_source_files_properties(
        "${_xllsdk_generic_srcdir}/generic.c" PROPERTIES
        LANGUAGE C
    )
    # make the extension .xll + never use unity build with generic XLL
    set_target_properties(generic PROPERTIES SUFFIX .xll UNITY_BUILD FALSE)
    # mark as found
    # note: not added to XLLSDK_LIBRARIES since generic.xll is not a library
    set(XLLSDK_Generic_FOUND TRUE)
endif()

# clean up
unset(_xllsdk_libdir)
unset(_xllsdk_root)
unset(_xllsdk_frmwrk_srcdir)
unset(_xllsdk_generic_srcdir)
unset(XLLSDK_XLCALL32)

# check version and required variables
find_package_handle_standard_args(
    XLLSDK
    # note: XLLSDK_LIBRARIES goes first so xlcall32.lib path shows in message
    REQUIRED_VARS XLLSDK_LIBRARIES XLLSDK_INCLUDE_DIRS
    VERSION_VAR XLLSDK_VERSION
    HANDLE_VERSION_RANGE
    HANDLE_COMPONENTS
)
