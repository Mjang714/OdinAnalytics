cmake_minimum_required(VERSION 3.20)

##
# oa_embed_version_info.cmake
#
# Provides a function to embed version info into OA binaries.
#
# Currently this only has an effect on Windows where the version resource is
# compiled into any specified binary targets.
#

include_guard(GLOBAL)

##
# Embed version information into the target binary.
#
# On Windows this embeds the resource info compiled from version.rc.in and
# appropriately defines any macros needed for the resource compiler invocation.
# The version.rc file is also appropriately included as a target source.
#
# Currently this function is a no-op on Linux. We may consider an alternate
# embedding scheme involving adding sections to an ELF object.
#
# Arguments:
#   target              Library or executable target
#
#   DESCRIPTION text    Brief file description. On Windows, the text will be
#                       displayed as the File Description field both when
#                       viewing the file properties and when hovering the mouse
#                       over the file in Explorer to view the info popup.
#
function(oa_embed_version_info target)
    cmake_parse_arguments(ARG "" "DESCRIPTION" "" ${ARGN})
    # check required arguments
    if(NOT ARG_DESCRIPTION)
        message(FATAL_ERROR "DESCRIPTION argument is required")
    endif()
    # if not on Windows do nothing
    if(NOT WIN32)
        return()
    endif()
    # get target type
    get_target_property(target_type ${target} TYPE)
    # determine what OA_RC_FILE_TYPE should be defined to
    # static library -> VFT_STATIC_LIB
    if(${target_type} STREQUAL "STATIC_LIBRARY")
        set(rc_file_type "VFT_STATIC_LIB")
    # dynamic library -> VFT_DLL
    elseif(
        ${target_type} STREQUAL "MODULE_LIBRARY" OR
        ${target_type} STREQUAL "SHARED_LIBRARY"
    )
        set(rc_file_type "VFT_DLL")
    # executable -> VFT_APP
    elseif(${target_type} STREQUAL "EXECUTABLE")
        set(rc_file_type "VFT_APP")
    # unsupported
    else()
        message(FATAL_ERROR "cannot use ${target_type} target with version.rc")
    endif()
    # set variables for configure_file() call on version.rc.in
    set(OA_TARGET "${target}")
    set(OA_RC_FILE_TYPE "${rc_file_type}")
    set(OA_RC_FILE_DESCRIPTION "${ARG_DESCRIPTION}")
    # current binary subdirectory
    cmake_path(
        RELATIVE_PATH CMAKE_CURRENT_BINARY_DIR
        BASE_DIRECTORY "${PROJECT_BINARY_DIR}"
        OUTPUT_VARIABLE out_subdir
    )
    # configure version.rc.in -> ${target}_version.rc
    #
    # variables consumed:
    #
    #   OA_TARGET
    #   OA_RC_FILE_TYPE
    #   OA_RC_FILE_DESCRIPTION
    #
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/version.rc.in" ${target}_version.rc
        @ONLY NEWLINE_STYLE LF
    )
    message(STATUS "Generated ${out_subdir}/${target}_version.rc")
    # add RC file to target sources + ensure generated oa/version.h included
    target_sources(
        ${target} PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/${target}_version.rc"
    )
    target_include_directories(${target} PRIVATE "${PROJECT_BINARY_DIR}/include")
    # compile with OA_RC_FILE_NAME defined for per-config file name
    # TODO: we can use generator expressions in compile definitions so maybe we
    # don't need to have any custom RC invocation rules. we may also not even
    # need a configure_file() step and could define more macros. however, the
    # downside is that OA_RC_FILE_NAME is also defined for C/C++ compilation,
    # which strictly speaking, is not something we want to happen
    target_compile_definitions(
        ${target} PRIVATE
        OA_RC_FILE_NAME="$<TARGET_FILE_NAME:${target}>"  # quotes preserved
    )
endfunction()
