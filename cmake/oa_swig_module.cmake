cmake_minimum_required(VERSION 3.21)

##
# oa_swig_module.cmake
#
# SWIGmodule building helpers that correctly track SWIG dependencies.
#
# This module is motivated as a lighter replacement to CMake's UseSWIG module
# which with Make as the build backend for some reason seems to lose tracking
# of implicit dependencies if SWIG errors out with CMake 4.l.
#

##
# Add SWIG compile options to the list of defaults.
#
# The semantics of this function are the same as add_compile_options().
#
# Options are picked up by any later target added with oa_swig_cxx_module().
#
function(oa_swig_compile_options)
    set(
        ODIN_SWIG_COMPILE_OPTIONS
        ${ODIN_SWIG_COMPILE_OPTIONS} ${ARGN} PARENT_SCOPE
    )
endfunction()

##
# Add SWIG compile definitions to the list of defaults.
#
# The semantics of this function are the same as add_compile_definitions().
#
# Definitions are picked up by any later target added with oa_swig_cxx_module().
#
# Note:
#
# SWIG 4.2 finally added the ability to add VAR=value definition. If this
# syntax is seen in any argument for SWIG < 4.2, a warning is emitted.
#
function(oa_swig_compile_definitions)
    # for SWIG < 4.2, emit a helpful warning on the NAME=value syntax
    if(SWIG_VERSION VERSION_LESS 4.2)
        foreach(arg ${ARGN})
            string(FIND "${arg}" "=" equals_pos)
            if(NOT equals_pos EQUAL -1)
                message(
                    WARNING
                    "${arg}: NAME=value macro definition cannot be used with "
"SWIG ${SWIG_VERSION} < 4.2"
                )
            endif()
        endforeach()
    endif()
    # prepend -D to every argument in the list that doesn't start with -D
    foreach(arg ${ARGN})
        string(FIND "${arg}" "-D" d_pos)
        if(NOT d_pos EQUAL 0)
            list(APPEND swig_defs "-D${arg}")
        else()
            list(APPEND swig_defs "${arg}")
        endif()
    endforeach()
    # add to existing list in parent scope
    set(
        ODIN_SWIG_COMPILE_DEFINITIONS
        ${ODIN_SWIG_COMPILE_DEFINITIONS} ${swig_defs} PARENT_SCOPE
    )
endfunction()

##
# Add a SWIG dynamic module for wrapping the given target language.
#
# By default, the generated .cxx file is written to CMAKE_CURRENT_BINARY_DIR,
# and the language-specific wrapper files will be placed in the same directory
# as the final dynamic module built from the SWIG-generated source file.
#
# Implicit dependencies are automatically tracked for optimal rebuild. The
# output library target will be of MODULE type. Additional target properties
# can be set to customize the build process, e.g.:
#
#   SWIG_COMPILE_DEFINITIONS        List of additional SWIG symbols to define.
#                                   Note that SWIG >=4.2 supports NAME=value.
#
#   SWIG_COMPILE_OPTIONS            List of additional SWIG compile options
#   SWIG_INCLUDE_DIRECTORIES        List of additional SWIG include directories
#
# By default the target's INCLUDE_DIRECTORIES property is forwarded to SWIG. To
# set properties on the SWIG module target and to link libraries, the normal
# usage of set_target_properties() and target_link_libraries() will work.
#
# This function has an advantage over swig_add_library() when using Makefile
# generators as its use of a custom target to drive the C/C++ generation
# ensures that when SWIG errors out, the build rule is correctly re-run. The
# custom target is always out-of-date, and since it is made to depend on the
# C/C++ generated file, it will ensure incremental builds work properly.
#
# Arguments:
#   target                  Target name
#   LANGUAGE lang           Target language option, e.g. python, csharp
#   MODULE                  SWIG .i module to compile
#
#   [OUTPUT_DIR dir]        Output directory for generated language-specific
#                           files, default is the location of ${target}
#
#   [OUTFILE_DIR dir]       Output directory for the generated .cxx wrapper
#                           file, default CMAKE_CURRENT_BINARY_DIR
#
#   [SOURCES sources...]    Additional C/C++ source files to compile
#   [CXX]                   Run SWIG in C++ wrapping mode
#   [DOXYGEN]               Run with -doxygen option for Python/Java
#
function(oa_swig_module target)
    # parse arguments
    cmake_parse_arguments(
        ARG
        "CXX;DOXYGEN" "LANGUAGE;MODULE;OUTPUT_DIR;OUTFILE_DIR" "SOURCES"
        ${ARGN}
    )
    # SWIG is of course required
    if(NOT SWIG_EXECUTABLE)
        message(FATAL_ERROR "SWIG_EXECUTABLE required for invoking SWIG")
    endif()
    # supported languages
    set(valid_langs python)
    # LANGUAGE is required + must be one of the supported languages
    if(NOT ARG_LANGUAGE)
        message(FATAL_ERROR "LANGUAGE argument required")
    endif()
    if(NOT ARG_LANGUAGE IN_LIST valid_langs)
        message(FATAL_ERROR "unsupported SWIG language ${ARG_LANGUAGE}")
    endif()
    # MODULE required
    if(NOT ARG_MODULE)
        message(FATAL_ERROR "MODULE argument required")
    endif()
    # OUTPUT_DIR defaults to the location of ${target}
    if(NOT ARG_OUTPUT_DIR)
        set(ARG_OUTPUT_DIR "$<TARGET_FILE_DIR:${target}>")
    endif()
    # OUTFILE_DIR defaults to CMAKE_CURRENT_BINARY_DIR
    if(NOT ARG_OUTFILE_DIR)
        set(ARG_OUTFILE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
    # build options from parameters
    if(ARG_CXX)
        list(APPEND swig_local_opts -c++)
        set(swig_ext ".cxx")
    else()
        set(swig_ext ".c")
    endif()
    # language-specific options
    if(ARG_LANGUAGE STREQUAL "python")
        # parse C++ Doxygen comments for docstrings
        if(ARG_DOXYGEN)
            list(APPEND swig_local_opts -doxygen)
        endif()
        # if SWIG < 4.2 we add the -py3 options
        if(SWIG_VERSION VERSION_LESS 4.2)
            list(APPEND swig_local_opts -py3)
        endif()
    endif()
    # MODULE target with the target sources
    add_library(${target} MODULE ${ARG_SOURCES})
    # uppercase language + filename + stem + absolute path of the SWIG module
    string(TOUPPER "${ARG_LANGUAGE}" lang_upper)
    cmake_path(GET ARG_MODULE FILENAME module_file)
    cmake_path(GET ARG_MODULE STEM module_stem)
    cmake_path(ABSOLUTE_PATH ARG_MODULE OUTPUT_VARIABLE module_path)
    # helper SWIG compile definitions + include directories genex
    set(swig_defs "$<TARGET_PROPERTY:${target},SWIG_COMPILE_DEFINITIONS>")
    set(swig_dirs "$<TARGET_PROPERTY:${target},SWIG_INCLUDE_DIRECTORIES>")
    set(tgt_includes "$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>")
    # output file path
    set(
        swig_output
        "${ARG_OUTFILE_DIR}/${module_stem}${lang_upper}_wrap${swig_ext}"
    )
    # custom command for the SWIG output file
    add_custom_command(
        OUTPUT "${swig_output}"
        COMMAND ${SWIG_EXECUTABLE}
                # SWIG compile definition defaults
                ${ODIN_SWIG_COMPILE_DEFINITIONS}
                # expand NAME[=value] defs into -DNAME[=value] if any
                $<$<BOOL:${swig_defs}>:-D$<JOIN:${swig_defs},$<SEMICOLON>-D>>
                # prepend -I onto SWIG + target include directories if any
                $<$<BOOL:${swig_dirs}>:-I$<JOIN:${swig_dirs},$<SEMICOLON>-I>>
                $<$<BOOL:${tgt_includes}>:-I$<JOIN:${tgt_includes},$<SEMICOLON>-I>>
                # SWIG compile option defaults + language + local options
                ${ODIN_SWIG_COMPILE_OPTIONS}
                -${ARG_LANGUAGE}
                ${swig_local_opts}
                # SWIG compile options on target
                $<TARGET_PROPERTY:${target},SWIG_COMPILE_OPTIONS>
                -MMD -MF ${CMAKE_CURRENT_BINARY_DIR}/${ARG_MODULE}.d
                -o ${swig_output}
                ${module_path}
        COMMENT "SWIG C++ compile for ${module_file}"
        DEPFILE ${CMAKE_CURRENT_BINARY_DIR}/${ARG_MODULE}.d
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    # add the SWIG output to the target
    target_sources(${target} PRIVATE "${swig_output}")
    # use custom target to drive the SWIG generation. the target is always out
    # of date so the SWIG output rule will be checked each build, ensuring that
    # the MODULE target is correctly rebuilt when SWIG errors. Makefile
    # generators seem to have a problem where if the target directly depends on
    # the SWIG .cxx file, if SWIG errored out and did not write a depfile, the
    # custom rule would not be reliably re-run, breaking incremental builds.
    add_custom_target(${target}_driver DEPENDS "${swig_output}")
    add_dependencies(${target} ${target}_driver)
endfunction()
