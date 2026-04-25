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
# On Windows this embeds the resource info compiled from version.rc into a .res
# resource file for the target and appropriately defines any macros needed for
# the resource compiler invocation. The version.rc file is also appropriately
# included as a target source and as a LINK.EXE input file.
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
    # indicate if multi-config generator + set .res file output directory
    get_property(is_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    set(res_outdir_genex "${CMAKE_CURRENT_BINARY_DIR}")
    if(is_multi)
        string(APPEND res_outdir_gene "/$<CONFIG>")
    endif()
    # target-specific resource file name
    set(res_outname "${target}_version.res")
    # custom command for .res file invoking CMAKE_RC_COMPILER
    add_custom_command(
        OUTPUT "${res_outdir_genex}/${res_outname}"
        COMMAND "${CMAKE_RC_COMPILER}"
                /nologo
                # repeat some standard Win32 compiler defines
                /DWIN32 /D_WIN32 /D_WINDOWS
                # define _DEBUG like MSVC if building Debug config
                $<$<CONFIG:Debug>:/D_DEBUG>
                # pass file type, file name, description through macro
                /DOA_RC_FILE_TYPE=${rc_file_type}
                # note: RC has some interesting escaping requirements. for
                # example, the quoting needs to be done *after* the option, so
                # for a quoted string specified with /D, we have to do:
                #
                #   /D"MY_MACRO=\"this is my string\""
                #
                # so in CMake land, we have the escape the \ as well. this
                # pattern is used for any quoted option argument.
                "/D\"OA_RC_FILE_NAME=\\\"$<TARGET_FILE_NAME:${target}>\\\"\""
                "/D\"OA_RC_FILE_DESCRIPTION=\\\"${ARG_DESCRIPTION}\\\"\""
                # default language is US English
                /l0x0409
                # ensure version.h is on include path
                "/I\"${PROJECT_BINARY_DIR}/include\""
                # output + input
                "/fo\"${res_outdir_genex}/${res_outname}\""
                "\"${PROJECT_SOURCE_DIR}/src/version.rc\""
        # hardcoded dependencies as RC doesn't do dependency scanning
        DEPENDS "${PROJECT_BINARY_DIR}/include/oa/version.h"
                "${PROJECT_SOURCE_DIR}/src/version.rc"
        COMMENT "RC compile version.rc into ${res_outname}"
        # note: don't use VERBATIM since RC has some weird behavior when it
        # comes to escaping arguments. see the above comment
    )
    # add .res resource file to list of target sources to trigger compile rule
    # but also add to linker inputs to force LINK.EXE to process the .res file
    # as by default, only .obj files are processed as linker input
    target_sources(${target} PRIVATE "${res_outdir_genex}/${res_outname}")
    target_link_libraries(${target} PRIVATE "${res_outdir_genex}/${res_outname}")
endfunction()
