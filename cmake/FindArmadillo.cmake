cmake_minimum_required(VERSION 3.22)

##
# FindArmadillo.cmake
#
# Find the Armadillo matrix library.
#
# This find module is provided to work around integration issues with CMake's
# own FindArmadillo.cmake find module as well as the upstream config script
# provided by the Armadillo library itself. Any BLAS, LAPACK, ARPACK, etc.
# functionality can be configured by the consuming project; this find module
# is concerned only with locating headers and if specified the wrapper.
#

include(FindPackageHandleStandardArgs)

# list of supported components
set(_armadillo_comps libarmadillo)

# check find components
foreach(comp ${Armadillo_FIND_COMPONENTS})
    list(FIND _armadillo_comps ${comp} comp_pos)
    if(comp_pos EQUAL -1)
        message(FATAL_ERROR "unknown component ${comp}")
    endif()
endforeach()

# locate main Armadillo header header
find_path(Armadillo_INCLUDE_DIRS armadillo)
if(NOT Armadillo_INCLUDE_DIRS)
    find_package_handle_standard_args(
        Armadillo
        REQUIRED_VARS Armadillo_INCLUDE_DIRS
    )
    return()
endif()

# locate Armadillo version header
file(
    STRINGS "${Armadillo_INCLUDE_DIRS}/armadillo_bits/arma_version.hpp"
    Armadillo_VERSION
    REGEX "#define[ \t]+ARMA_VERSION_(MAJOR|MINOR|PATCH)[ \t]+[0-9]+"
)
# keep only numeric values + join into major.minor.patch string
list(TRANSFORM Armadillo_VERSION REPLACE "#define[ \t]+[A-Z_]+[ \t]+" "")
list(JOIN Armadillo_VERSION "." Armadillo_VERSION)

# look for wrapper library
cmake_path(GET Armadillo_INCLUDE_DIRS PARENT_PATH _armadillo_root)
find_library(
    Armadillo_LIBRARY armadillo NO_DEFAULT_PATH
    PATHS "${_armadillo_root}/lib"
)
# indicate component found if appropriate
if(Armadillo_LIBRARY)
    set(Armadillo_libarmadillo_FOUND TRUE)
endif()

# create imported target
add_library(Armadillo::Armadillo INTERFACE IMPORTED)
target_include_directories(
    Armadillo::Armadillo INTERFACE
    ${Armadillo_INCLUDE_DIRS}
)
# add wrapper if found and desired
list(FIND Armadillo_FIND_COMPONENTS "libarmadillo" libarma_pos)
if(NOT libarma_pos EQUAL -1 AND Armadillo_LIBRARY)
    # note: ensures that ARMA_USE_WRAPPER defined even if the
    # armadillo_bits/config.hpp header doesn't define it
    target_compile_definitions(Armadillo::Armadillo INTERFACE ARMA_USE_WRAPPER)
    target_link_libraries(Armadillo::Armadillo INTERFACE ${Armadillo_LIBRARY})
    unset(libarma_pos)
# otherwise defaults to no wrapper
else()
    target_compile_definitions(
        Armadillo::Armadillo INTERFACE
        ARMA_DONT_USE_WRAPPER
    )
endif()

# check required variables and version
find_package_handle_standard_args(
    Armadillo
    REQUIRED_VARS Armadillo_INCLUDE_DIRS
    VERSION_VAR Armadillo_VERSION
    HANDLE_VERSION_RANGE
    HANDLE_COMPONENTS
)
