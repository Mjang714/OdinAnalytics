cmake_minimum_required(VERSION 3.15)

include_guard(GLOBAL)

##
# oa_pdb_install.cmake
#
# Provide an installation rule for targets compiled using MSVC for PDB install.
#
# The function correctly handles differences in which PDB file to select when
# using static or shared targets and allows specifying configs + components.
#

##
# Obtain the ARCHIVE target output name for the specified build config.
#
# This performs a search for the following names:
#
#   ARCHIVE_OUTPUT_NAME_<CONFIG>
#   ARCHIVE_OUTPUT_NAME
#   OUTPUT_NAME_<CONFIG>
#   OUTPUT_NAME
#   NAME
#
# NAME is always guaranteed. We stop the search at the first value available.
#
# Arguments:
#   out             Output variable
#   TARGET tgt      Target to examine
#   CONFIG cfg      Build configuration to consider
#
function(oa_archive_output_name out)
    # parse arguments
    cmake_parse_arguments(ARG "" "TARGET;CONFIG" "" ${ARGN})
    # required
    if(NOT ARG_TARGET)
        message(FATAL_ERROR "TARGET is required")
    endif()
    if(NOT ARG_CONFIG)
        message(FATAL_ERROR "CONFIG is required")
    endif()
    # upper-case config
    string(TOUPPER "${ARG_CONFIG}" upper_config)
    # get any config-specific postfix
    get_target_property(postfix ${ARG_TARGET} ${upper_config}_POSTFIX)
    # might be postfix-NOTFOUND so explicitly set to empty string
    if(NOT postfix)
        set(postfix "")
    endif()
    # try the different properties in order
    foreach(
        prop
            ARCHIVE_OUTPUT_NAME_${upper_config}
            ARCHIVE_OUTPUT_NAME
            OUTPUT_NAME_${upper_config}
            OUTPUT_NAME
            NAME
    )
        # if successful, set property and return
        # note: guaranteed, as NAME is always present
        get_target_property(out_name ${ARG_TARGET} ${prop})
        if(out_name)
            set(${out} "${out_name}${postfix}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
endfunction()

##
# Provide an installation rule for the PDB file of a target compiled with MSVC.
#
# This function is a no-op when MSVC is not being used as the compiler. For
# static libraries the PDB file generated from /Fd is used while for shared
# libraries the linker-generated PDB file is used.
#
# The PDB install destination is bin by default but can be overridden. Usually
# you want the PDB to be installed in the runtime output directory with DLLs so
# that debuggers can locate the PDB without needing a search path specified.
#
# Note:
#
# This function will force static libraries compiled with MSVC to create a PDB
# file with the same name by setting COMPILE_PDB_NAME_<CONFIG> to the target's
# name as determined by oa_archive_output_name(). The PDB file will also be
# forced to be located in the same directory as the target by setting
# COMPILE_PDB_OUTPUT_DIRECTORY_<CONFIG> to $<TARGET_FILE_DIR:${target}>.
#
# Arguments:
#   target                      Name of target
#
#   CONFIGURATIONS cfgs...      List of build configurations to install PDBs
#                               for. This is typically Debug, RelWithDebInfo,
#                               or custom build configs with debugging info.
#
#   [DESTINATION dest]          Install subdirectory, e.g. bin (the default)
#   [COMPONENT component]       Install component to associate PDB file with
#
function(oa_pdb_install target)
    # parse arguments
    cmake_parse_arguments(ARG "" "DESTINATION;COMPONENT" "CONFIGURATIONS" ${ARGN})
    # CONFIGURATIONS is required
    if(NOT ARG_CONFIGURATIONS)
        message(FATAL_ERROR "CONFIGURATIONS requires at least one config value")
    endif()
    # DESTINATION default is bin
    if(NOT ARG_DESTINATION)
        set(ARG_DESTINATION bin)
    endif()
    # if COMPONENT is provided, build optional args
    if(ARG_COMPONENT)
        set(component_args COMPONENT ${ARG_COMPONENT})
    endif()
    # if not compiling with MSVC exit early. PDB will not be applicable
    if(NOT MSVC)
        return()
    endif()
    # determine if library is static or shared
    get_target_property(target_type ${target} TYPE)
    # if shared library or module just use TARGET_PDB_FILE
    if(
        target_type STREQUAL "SHARED_LIBRARY" OR
        target_type STREQUAL "MODULE_LIBRARY"
    )
        install(
            FILES $<TARGET_PDB_FILE:${target}>
            DESTINATION ${ARG_DESTINATION}
            CONFIGURATIONS ${ARG_CONFIGURATIONS}
            ${component_args}
        )
    # if static library we use the output directory and name per config
    elseif(target_type STREQUAL "STATIC_LIBRARY")
        foreach(config ${ARG_CONFIGURATIONS})
            # all-caps config name
            string(TOUPPER "${config}" upper_config)
            # find static library output name
            oa_archive_output_name(output_name TARGET ${target} CONFIG ${config})
            # force PDB name to be the same name in same directory
            set_target_properties(
                ${target} PROPERTIES
                COMPILE_PDB_NAME_${upper_config} "${output_name}"
                COMPILE_PDB_OUTPUT_DIRECTORY_${upper_config}
                    $<TARGET_FILE_DIR:${target}>
            )
            # add install rule for /Fd PDB
            install(
                FILES "$<TARGET_FILE_DIR:${target}>/${output_name}.pdb"
                DESTINATION ${ARG_DESTINATION}
                CONFIGURATIONS ${ARG_CONFIGURATIONS}
                ${component_args}
            )
        endforeach()
    # error
    else()
        message(
            FATAL_ERROR
            "target TYPE must be STATIC_LIBRARY, SHARED_LIBRARY, or MODULE_LIBRARY"
        )
    endif()
endfunction()
