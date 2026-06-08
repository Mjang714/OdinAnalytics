cmake_minimum_required(VERSION 3.20)

##
# FindR.cmake
#
# Locate the R interpreter and development artifacts for Linux and Windows.
#
# The following components are supported:
#
#   Interpreter             The interactive R and Rscript executables
#
#   Development             R development headers and libraries. This component
#                           is never found on Windows because the R Windows
#                           toolchain uses MinGW which is ABI-incompatible with
#                           MSVC, which makes extension development difficult.
#
# On success, this module sets R_FOUND to TRUE. If the Interpreter component is
# found, then the following variables are defined:
#
#   R_EXECUTABLE            Path to the interactive R interpreter
#   R_SCRIPT_EXECUTABLE     Path to the Rscript R scripting frontend
#   R_VERSION               R major.minor.patch version
#   R_64BIT                 TRUE if R interpreter is 64-bit
#   R_Interpreter_FOUND     TRUE to indicate Interpreter component found
#
# The following targets will also be defined:
#
#   R::Rscript              Rscript executable target. There is no target for
#                           the interactive R interpreter.
#
# If the Development component is found, the following variables are defined:
#
#   R_INCLUDE_DIR           Path to the R header include directory. This is
#                           always available in a base R installation.
#
#   R_LIBRARY               Path to the R runtime library. Not available on
#                           Windows due to the aforementioned ABI issues.
#
#   R_Development_FOUND     TRUE to indicate Development component found
#
# The following targets will also be defined:
#
#   R::R                    R runtime library target for extension building
#

include(FindPackageHandleStandardArgs)

# supported components
set(r_supported_components Interpreter Development)

# check requested components
foreach(_comp ${R_FIND_COMPONENTS})
    if(NOT ${_comp} IN_LIST r_supported_components)
        message(FATAL_ERROR "unsupported R component ${_comp}")
    endif()
endforeach()

# path suffixes for searching for executables for Interpreter component
# note: bin is standard but we need a nonempty list
list(APPEND _r_bindirs bin)
if(WIN32)
    list(PREPEND _r_bindirs bin/x64)
endif()

# look for R executable as Interpreter component is always searched
find_program(R_EXECUTABLE R PATH_SUFFIXES ${_r_bindirs} NO_CACHE)
if(NOT R_EXECUTABLE)
    find_package_handle_standard_args(R REQUIRED_VARS R_EXECUTABLE)
    return()
endif()

# determine the version of the R executable
execute_process(
    COMMAND "${R_EXECUTABLE}" --version
    # note: on Linux R --version outputs to stdout but on Windows to stderr
    OUTPUT_VARIABLE R_VERSION
    ERROR_VARIABLE R_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)
# keep only major.minor.patch version
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" R_VERSION "${R_VERSION}")
# check if the version if valid (non-empty)
if(NOT R_VERSION)
    find_package_handle_standard_args(R REQUIRED_VARS R_VERSION)
    return()
endif()

# look for Rscript executable
find_program(R_SCRIPT_EXECUTABLE Rscript PATH_SUFFIXES ${r_bindirs} NO_CACHE)
if(NOT R_SCRIPT_EXECUTABLE)
    find_package_handle_standard_args(R REQUIRED_VARS R_SCRIPT_EXECUTABLE)
    return()
endif()

# write + execute the command to print the pointer size
file(
    WRITE "${PROJECT_BINARY_DIR}/pointersize.R"
    "cat(.Machine$sizeof.pointer, fill = TRUE)"
)
execute_process(
    COMMAND "${R_SCRIPT_EXECUTABLE}" "${PROJECT_BINARY_DIR}/pointersize.R"
    OUTPUT_VARIABLE R_64BIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)
# if 8 then R_64BIT is TRUE
if(R_64BIT STREQUAL "8")
    set(R_64BIT TRUE)
else()
    set(R_64BIT FALSE)
endif()

# create R::Rscript target and mark Interpreter component as found
add_executable(R::Rscript IMPORTED)
set_target_properties(
    R::Rscript PROPERTIES
    IMPORTED_LOCATION "${R_SCRIPT_EXECUTABLE}"
)
set(R_Interpreter_FOUND TRUE)

# look for R headers. on Linux these are stored in <prefix>/share/R/include
if(WIN32)
    set(_r_includes include)
else()
    set(_r_includes share/R/include)
endif()
# note: include directory is included by default in an R installation
find_path(R_INCLUDE_DIR R.h PATH_SUFFIXES ${_r_includes} NO_CACHE)

# look for the R runtime library. on Windows we can't use this for development
# as besides ABI issues we need an import library to link against the DLL,
# which is what this function is looking for (and will not find)
if(WIN32)
    set(_r_library R)
# note: R runtime library could be static apparently
else()
    set(_r_library libR.so libR.a)
endif()
# note: on Linux the lib/libR.so is actually symlinked to lib/R/lib/libR.so
find_library(R_LIBRARY NAMES ${_r_library} NO_CACHE)

# if both R include directory and library are found
if(R_INCLUDE_DIR AND R_LIBRARY)
    # get extension of R library + create library target as static/shared
    cmake_path(GET R_LIBRARY EXTENSION _r_library_ext)
    if(_r_library_ext STREQUAL ".a")
        add_library(R::R STATIC IMPORTED)
    else()
        add_library(R::R SHARED IMPORTED)
    endif()
    # set library location + includes
    set_target_properties(R::R PROPERTIES IMPORTED_LOCATION "${R_LIBRARY}")
    target_include_directories(R::R INTERFACE "${R_INCLUDE_DIR}")
    # mark Development component as found
    set(R_Development_FOUND TRUE)
endif()

# handle all components, version range, etc.
find_package_handle_standard_args(
    R
    REQUIRED_VARS R_EXECUTABLE R_SCRIPT_EXECUTABLE
    VERSION_VAR R_VERSION
    HANDLE_VERSION_RANGE
    HANDLE_COMPONENTS
)
