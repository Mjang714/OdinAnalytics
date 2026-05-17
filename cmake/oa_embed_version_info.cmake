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
# For Unix-like systems, with the implicit assumption that the object file
# format is ELF, the version_elf.cpp file is separately compiled and linked for
# the specified target with appropriate compile definitions. version_elf.cpp
# embeds version information in extra ELF object sections that are prefixed
# with "oa." to distinguish them from other sections.
#
# For details on the VERSIONINFO specification on Windows and the ELF sections
# used on *nix systems, see version.rc and version_elf.cpp respectively.
#
# Arguments:
#   target              Library or executable target
#
#   DESCRIPTION text    Brief file description. On Windows, the text will be
#                       displayed as the File Description field both when
#                       viewing the file properties and when hovering the mouse
#                       over the file in Explorer to view the info popup. The
#                       text will also be available via the OA_DESCRIPTION
#                       property that will be added to the target.
#
function(oa_embed_version_info target)
    cmake_parse_arguments(ARG "" "DESCRIPTION" "" ${ARGN})
    # check required arguments
    if(NOT ARG_DESCRIPTION)
        message(FATAL_ERROR "DESCRIPTION argument is required")
    endif()
    # set OA_DESCRIPTION using description text
    set_target_properties(
        ${target} PROPERTIES
        OA_DESCRIPTION "${ARG_DESCRIPTION}"
    )
    # if not on Windows use version_elf.cpp to create a separate object library
    if(NOT WIN32)
        add_library(
            ${target}_elf_version OBJECT
            ${PROJECT_SOURCE_DIR}/src/version_elf.cpp
        )
        # ensure that regardless of the target's own includes that
        # version_elf.cpp can include oa/version.h and oa/common.h
        target_include_directories(
            ${target}_elf_version PRIVATE
            ${PROJECT_BINARY_DIR}/include ${PROJECT_SOURCE_DIR}/include
        )
        # also need to compile with OA_RC_FILE_DESCRIPTION and OA_RC_FILE_NAME
        # for correct file name + description embedding
        target_compile_definitions(
            ${target}_elf_version PRIVATE
            OA_RC_FILE_DESCRIPTION="${ARG_DESCRIPTION}"
            OA_RC_FILE_NAME="$<TARGET_FILE_NAME:${target}>"
        )
        # ensure target picks up the object as part of the link
        target_sources(${target} PRIVATE $<TARGET_OBJECTS:${target}_elf_version>)
        # done
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
        string(APPEND res_outdir_genex "/$<CONFIG>")
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
        COMMENT "RC compile version.rc"
        # note: don't use VERBATIM since RC has some weird behavior when it
        # comes to escaping arguments. see the above comment
    )
    # add .res resource file to list of target sources to trigger compile rule
    # but also add to linker inputs to force LINK.EXE to process the .res file
    # as by default, only .obj files are processed as linker input
    target_sources(${target} PRIVATE "${res_outdir_genex}/${res_outname}")
    target_link_libraries(${target} PRIVATE "${res_outdir_genex}/${res_outname}")
endfunction()

##
# Add a test to validate the embedded version information with oa-dumpver.
#
# This function uses add_test() to add a test that calls oa-dumpver on the
# specified target's output file and then checks that all the retrieved version
# information matches what has been configured during the build.
#
# The target's OA_DESCRIPTION property is read to get the description text.
#
# Arguments:
#   target      Library or executable target with embedded version info
#
function(oa_check_version_info target)
    # get OA_DESCRIPTION property from target for description text
    get_target_property(description ${target} OA_DESCRIPTION)
    # use ODIN_(MAJOR|MINOR|PATCH)_VERSION values to construct 64-bit hex value
    math(
        EXPR hex_version
        "(${ODIN_MAJOR_VERSION} << 0x30) + (${ODIN_MINOR_VERSION} << 0x20)"
        OUTPUT_FORMAT HEXADECIMAL
    )
    math(
        EXPR hex_version
        "${hex_version} + (${ODIN_PATCH_VERSION} << 0x10)"
        OUTPUT_FORMAT HEXADECIMAL
    )
    # pad hex value with leading zeros so total length is 18 characters. the
    # extra two characters come from the leading "0x" prepended
    string(LENGTH "${hex_version}" hex_version_len)
    math(EXPR version_padding "18 - ${hex_version_len}")
    # need to pad with zeroes
    if(version_padding GREATER 0)
        string(REPEAT "0" ${version_padding} version_zeroes)
        string(REPLACE "0x" "0x${version_zeroes}" hex_version "${hex_version}")
    endif()
    string(
        CONCAT test_regex
        "product:[ ]+OA Software Suite\n"
        "company:[ ]+OA Developers\n"
        "description:[ ]+${description}\n"
        "version number:[ ]+${hex_version}\n"
        "version string:[ ]+${ODIN_VERSION}\n"
        "original name:[ ]+$<TARGET_FILE_NAME:${target}>\n"
    )
    # add oa_dumpver test for target + set pass regex to test_regex
    add_test(
        NAME ${target}_version_check
        COMMAND oa_dumpver $<TARGET_FILE:${target}>
    )
    set_tests_properties(
        ${target}_version_check PROPERTIES
        PASS_REGULAR_EXPRESSION "${test_regex}"
    )
endfunction()
