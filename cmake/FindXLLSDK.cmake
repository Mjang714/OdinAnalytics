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
#   XLLSDK::xlcall32        Target for linking against xlcall32.lib
#
# The following CMake ALIAS static library target will also be defined:
#
#   XLLSDK::frmwrk32        Target for linking against frmwrk32.lib
#
# Selection of 32-bit and 64-bit libraries is automatically done.
#
# Note that the frmwrk32 target itself will be an alias to an actual target
# added to the calling project's build tree. This is because in 2025 the
# Microsoft-provided NMake Makefiles in the Excel2013XLLSDK\SAMPLES subdirs
# are out of date and won't built under newer Visual Studio compiler versions,
# e.g. 2022. The BUILD.BAT and MAKE.BAT have the additional issue of looking
# for vcvarsalls.bat in %VCINSTALLDIR% directly when this script has now been
# moved to %VCINSTALLDIR%\Auxiliary\Build instead.
#
# TODO:
#
# Support COMPONENTS, e.g. enable find_package usage like the following:
#
#   find_package(XLLSDK 15.0 COMPONENTS XLCALL32 FRMWRK32)
#
# This way we don't need to force consumers to have the SAMPLES\FRAMEWRK source
# directory available if they have xlcall32.lib in a vendor install root.
#

include(FindPackageHandleStandardArgs)

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

# locate xlcall32
find_library(
    XLLSDK_XLCALL32 xlcall32.lib
    PATH_SUFFIXES ${_xllsdk_libdir}
    NO_CACHE
)
if(NOT XLLSDK_XLCALL32)
    find_package_handle_standard_args(XLLSDK REQUIRED_VARS XLLSDK_XLCALL32)
    return()
endif()

# define xlcall32 IMPORTED target
add_library(XLLSDK::xlcall32 STATIC IMPORTED)
target_include_directories(XLLSDK::xlcall32 INTERFACE "${XLLSDK_INCLUDE_DIRS}")
set_target_properties(
    XLLSDK::xlcall32 PROPERTIES
    IMPORTED_LOCATION "${XLLSDK_XLCALL32}"
)

# get root install directory + framework library source directory
set(_xllsdk_root "${XLLSDK_INCLUDE_DIRS}/..")
set(_xllsdk_frmwrk_srcdir "${_xllsdk_root}/samples/framewrk")

# directly build frmwrk32 as part of the calling project
add_library(
    frmwrk32 STATIC
    "${_xllsdk_frmwrk_srcdir}/framewrk.c"
    "${_xllsdk_frmwrk_srcdir}/MemoryManager.cpp"
    "${_xllsdk_frmwrk_srcdir}/MemoryPool.cpp"
)
add_library(XLLSDK::frmwrk32 ALIAS frmwrk32)
# note: not defining _USRDLL during compilation. do we need to (for MFC)?
target_include_directories(
    frmwrk32 PRIVATE
    # needs to be able to include xlcall.cpp and xlcall.h
    "${XLLSDK_INCLUDE_DIRS}"
    "${_xllsdk_root}/src"
    # note: explicitly include _xllsdk_frmwrk_srcdir due to use of
    # <memorymanager.h> include and CMake won't be running in same directory
    "${_xllsdk_frmwrk_srcdir}"
)
# explicitly need to compile FRAMEWRK.C as C code
set_source_files_properties(
    "${_xllsdk_frmwrk_srcdir}/framewrk.c" PROPERTIES
    LANGUAGE C
)
# ensure we *never* use unity build when building frmwrk32
set_target_properties(frmwrk32 PROPERTIES UNITY_BUILD FALSE)

# all library targets
set(XLLSDK_LIBRARIES "${XLLSDK_XLCALL32};XLLSDK::frmwrk32")

# clean up
unset(_xllsdk_libdir)
unset(_xllsdk_root)
unset(_xllsdk_frmwrk_srcdir)
unset(XLLSDK_XLCALL32)

# check version and required variables
find_package_handle_standard_args(
    XLLSDK
    # note: XLLSDK_LIBRARIES goes first so xlcall32.lib path shows in message
    REQUIRED_VARS XLLSDK_LIBRARIES XLLSDK_INCLUDE_DIRS
    VERSION_VAR XLLSDK_VERSION
    HANDLE_VERSION_RANGE
)
