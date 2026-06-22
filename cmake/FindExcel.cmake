cmake_minimum_required(VERSION 3.20)

##
# FindExcel.cmake
#
# Locates a Microsoft Excel installation using some heuristics.
#
# On success, the following variables are defined:
#
#   Excel_EXECUTABLE        Path to the Excel executable
#   Excel_INSTALL_ROOT      Path to the Excel/Office installation root
#   Excel_VERSION           Excel Office version, e.g. 13, 16
#   Excel_64BIT             TRUE if Excel is 64-bit
#
# The Microsoft Office installation root typically contains XLCALL32.DLL which
# is the runtime DLL required when using the Excel XLL SDK XLCALL32.LIB import
# library. If found, the following variable will be defined:
#
#   Excel_XLCALL32_DLL      Path to the XLCALL32 DLL
#
# The following IMPORTED MODULE target will also be defined:
#
#   Excel::XLCALL32
#
# For applications that link against XLCALL32.LIB, e.g. linking against the
# XLLSDK::SDK import library provided by FindXLLSDK.cmake, one can add a custom
# post-build event using add_custom_command() to copy_if_different XLCALL32.DLL
# to $<TARGET_FILE_DIR:tgt> for a target that needs XLCALL32.DLL at runtime.
#
# Example:
#
#   # at least Office 2013
#   find_package(Excel 13 REQUIRED)
#   # check for XLCALL32.DLL
#   if(TARGET Excel::XLCALL32)
#       message(STATUS "XLCALL32 DLL: ${Excel_XLCALL32_DLL}")
#   endif()
#

include(FindPackageHandleStandardArgs)

# search for a few hardcoded versions
foreach(ver 12 13 14 15 16)
    # look for 64-bit first
    find_program(
        Excel_EXECUTABLE EXCEL
        HINTS "C:/Program Files/Microsoft Office"
        PATH_SUFFIXES Office${ver} root/Office${ver}
        NO_CACHE
    )
    # if found set variables + break loop
    if(Excel_EXECUTABLE)
        set(Excel_VERSION ${ver})
        set(Excel_64BIT TRUE)
        break()
    endif()
    # look for 32-bit
    find_program(
        Excel_EXECUTABLE EXCEL
        HINTS "C:/Program Files (x86)/Microsoft Office"
        PATH_SUFFIXES Office${ver} root/Office${ver}
        NO_CACHE
    )
    # if found set variables + break loop
    if(Excel_EXECUTABLE)
        set(Excel_VERSION ${ver})
        set(Excel_64BIT FALSE)
        break()
    endif()
endforeach()

# if not found, error
if(NOT Excel_EXECUTABLE)
    find_package_handle_standard_args(Excel REQUIRED_VARS Excel_EXECUTABLE)
    return()
endif()

# get installation directory + look for XLCALL32.DLL
cmake_path(GET Excel_EXECUTABLE PARENT_PATH Excel_INSTALL_ROOT)
find_file(
    Excel_XLCALL32_DLL XLCALL32.DLL
    HINTS "${Excel_INSTALL_ROOT}"
    NO_CACHE
    NO_DEFAULT_PATH
)
# if found, add module target
# note: this is actually the runtime DLL required by XLCALL32.LIB. we could
# actually consider XLLSDK::SDK to be a single SHARED library, but since
# XLCALL32.DLL is distributed with Excel while XLCALL32.LIB is part of the
# Excel 2013 XLL SDK, we just treat XLCALL32 as MODULE
if(Excel_XLCALL32_DLL)
    add_library(Excel::XLCALL32 MODULE IMPORTED)
    set_target_properties(
        Excel::XLCALL32 PROPERTIES
        IMPORTED_LOCATION "${Excel_XLCALL32_DLL}"
    )
endif()

# handle found variables
find_package_handle_standard_args(
    Excel
    REQUIRED_VARS Excel_EXECUTABLE Excel_INSTALL_ROOT
    VERSION_VAR Excel_VERSION
)
