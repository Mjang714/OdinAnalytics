cmake_minimum_required(VERSION 3.20)

##
# oa_r_package.cmake
#
# Module providing helpers for R packages with or without binary artifacts.
#
# In particular, the packages are built out-of-tree, keeping the source clean.
#

##
# Configure any .in files in the R package tree.
#
# The .in files can be src/Makevars.in or configure.in for example.
#
# Required variables:
#
#   MAKEVARS_IN             Makevars.in file in the build tree
#   MAKEVARS_OUT            Makevars file in the build tree
#
# Optional variables:
#
#   PKG_CPPFLAGS            For Makevars.in @PKG_CPPFLAGS@
#   PKG_CXXFLAGS            For Makevars.in @PKG_CXXFLAGS@
#   PKG_LIBS                For Makevars.in @PKG_LIBS@
#
function(oa_configure_r_package)
    # check required variables
    if(NOT MAKEVARS_IN)
        message(FATAL_ERROR "MAKEVARS_IN required")
    endif()
    if(NOT MAKEVARS_OUT)
        message(FATAL_ERROR "MAKEVARS_OUT required")
    endif()
    # configure file
    configure_file("${MAKEVARS_IN}" "${MAKEVARS_OUT}" @ONLY NEWLINE_STYLE LF)
endfunction()

##
# Install an R package into the build directory package root.
#
# This functions adds a custom command that calls R CMD INSTALL on a build tree
# copy of the given R package located in the source tree. The final package
# will end up installed into ${PROJECT_BINARY_DIR}/R.
#
# For integration with objects and libraries built using CMake with the R CMD
# INSTALL build system, the R package source tree must contain one .c or .cpp
# file in its src directory and contain a Makevars.in for CMake to configure.
#
# The following variables can be configured by CMake for the Makevars.in:
#
#   @PKG_CPPFLAGS@      List of preprocessor flags. This includes -I<dir>
#                       options for include directories and is populated using
#                       target INTERFACE_INCLUDE_DIRECTORIES and
#                       INTERFACE_COMPILE_DEFINITIONS values.
#
#   @PKG_CFLAGS@        List of C compiler flags. This is populated using the
#                       INTERFACE_COMPILE_OPTIONS of the targets.
#
#   @PKG_CXXFLAGS@      List of C++ compiler flags. This is populated using the
#                       INTERFACE_COMPILE_OPTIONS of the targets.
#
#   @PKG_LIBS@          List of linker flags and argumennts. This can include
#                       object files from the TARGET_OBJECTS generator
#                       expression, static or shared libraries, etc. It is
#                       recommended that -Wl,--whole-archive and
#                       -Wl,--no-whole-archive are used to ensure that any
#                       static libraries have all their objects included.
#
#                       This is populated with the INTERFACE_LINK_OPTIONS and
#                       INTERFACE_LINK_LIBRARIES properties of the targets.
#
# This function requires the R::R target added by the FindR.cmake find module.
#
# Arguments:
#   target              Target name
#
#   PACKAGE package     R package name corresponding to the name of a directory
#                       in the current CMake source directory
#
#   FILES files         R package files. This must contain *all* files that are
#                       in the package source tree, which includes the required
#                       DESCRIPTION and NAMESPACE files. The package structure
#                       must be valid, i.e. R files are in the R subdirectory,
#                       C/C++ files in src, etc. Generated files that should be
#                       included in the final package should be provided by a
#                       target that depends on the <snake-package>_copy target
#                       that copies the package tree to the current binary
#                       directory. Here <snake-package> is the package name
#                       with underscores in the place of any non-alphanumeric
#                       characters, e.g. foo.bar -> foo_bar.
#
#   [LIBRARIES libs...]
#
#                       Shared or static library targets to link against or
#                       include as part of the final DSO. For example, static
#                       libraries are linked as whole archive. The properties
#                       on the targets will be used to populate the PKG_*
#                       variables in the Makevars.in for R CMD INSTALL.
#
#   [NO_TEST_LOAD]      Skip test loading of the installed package which can be
#                       useful during builds where LD_LIBRARY_PATH is not set
#
#   [LTO (ON|OFF)]      Whether link-time optimization should be used
#
function(oa_r_package target)
    # parse + check unparsed arguments
    cmake_parse_arguments(
        ARG
        "NO_TEST_LOAD"
        "PACKAGE;PREFIX;LTO"
        "FILES;LIBRARIES"
        ${ARGN}
    )
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "unparsed arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    # package is required
    if(NOT ARG_PACKAGE)
        message(FATAL_ERROR "PACKAGE required")
    endif()
    # source + build tree package directories
    set(pkg_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_PACKAGE}")
    set(pkg_build_dir "${CMAKE_CURRENT_BINARY_DIR}/${ARG_PACKAGE}")
    # prepend package name to each source file for correct absolute paths
    list(
        TRANSFORM ARG_FILES
        PREPEND "${pkg_source_dir}/"
        OUTPUT_VARIABLE pkg_source_files
    )
    list(
        TRANSFORM ARG_FILES
        PREPEND "${pkg_build_dir}/"
        OUTPUT_VARIABLE
        pkg_build_files
    )
    # R library in build directory
    set(r_prefix "${PROJECT_BINARY_DIR}/R")
    # initial target to ensure r_prefix exists. this is added only if it
    # doesn't exist and is used to prevent racing for the directory creation
    if(NOT TARGET oa_r_prefix)
        add_custom_target(
            oa_r_prefix
            COMMAND ${CMAKE_COMMAND} -E make_directory ${r_prefix}
            VERBATIM
        )
    endif()
    # separate custom command for copying the package source tree to the binary
    # directory. although not strictly necessary it is a useful synchronization
    # point for custom commands that generate files intended for inclusion into
    # the build tree copy of the R package for R CMD INSTALL
    add_custom_command(
        OUTPUT ${pkg_build_files}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${pkg_source_dir} ${pkg_build_dir}
        # note: could glob for convenience but IDE builds won't automatically
        # reconfigure to pick up added files since the CMakeLists.txt would not
        # have changed. in general globbing is discouraged for this reason
        DEPENDS ${pkg_source_files}
        # note: need to explicitly set to "" to disable message
        COMMENT ""
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    # snake-case name for target
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" clean_package "${ARG_PACKAGE}")
    # as usual drive with custom target. depends on oa_r_prefix
    add_custom_target(${clean_package}_copy DEPENDS ${pkg_build_files})
    add_dependencies(${clean_package}_copy oa_r_prefix)
    # R CMD INSTALL options
    if(ARG_NO_TEST_LOAD)
        list(APPEND r_options --no-test-load)
    endif()
    # add a build command to configure Makevars if it exists
    if(EXISTS ${pkg_build_dir}/src/Makevars.in)
        # collect all library target properties
        set(cur_libs ${ARG_LIBRARIES})
        # loop through all target dependencies
        while(cur_libs)
            foreach(lib ${cur_libs})
                # lib is an actual TARGET
                if(TARGET ${lib})
                    # if INTERFACE target, skip
                    get_target_property(lib_type ${lib} TYPE)
                    if(lib_type STREQUAL INTERFACE_LIBRARY)
                        continue()
                    endif()
                    # collect preprocessor flags
                    get_target_property(
                        lib_defs ${lib}
                        INTERFACE_COMPILE_DEFINITIONS
                    )
                    if(lib_defs)
                        list(APPEND all_defs ${lib_defs})
                    endif()
                    get_target_property(
                        lib_includes ${lib}
                        INTERFACE_INCLUDE_DIRECTORIES
                    )
                    if(lib_includes)
                        list(APPEND all_includes ${lib_includes})
                    endif()
                    # collect compile flags
                    get_target_property(
                        lib_compile_options ${lib}
                        INTERFACE_COMPILE_OPTIONS
                    )
                    if(lib_compile_options)
                        list(APPEND all_compile_options ${lib_compile_options})
                    endif()
                    # collect linker flags
                    get_target_property(
                        lib_link_options ${lib}
                        INTERFACE_LINK_OPTIONS
                    )
                    if(lib_link_options)
                        list(APPEND all_link_options ${lib_link_options})
                    endif()
                    get_target_property(
                        lib_link_libs ${lib}
                        INTERFACE_LINK_LIBRARIES
                    )
                    if(lib_link_libs)
                        list(APPEND all_link_libs ${lib_link_libs})
                    endif()
                    # use target file name generator expression
                    list(APPEND all_link_options "$<TARGET_FILE:${lib}>")
                else()
                    list(APPEND all_link_options ${lib})
                endif()
            endforeach()
            # reset cur_libs to all_link_libs + clear all_link_libs. this
            # allows us to iteratively reduce the dependent targets
            set(cur_libs ${all_link_libs})
            unset(all_link_libs)
        endwhile()
        # remove duplicate options
        list(REMOVE_DUPLICATES all_defs)
        list(REMOVE_DUPLICATES all_includes)
        list(REMOVE_DUPLICATES all_compile_options)
        list(REMOVE_DUPLICATES all_link_options)
        # generator expressions for defs and includes
        set(
            all_defs_gen
            "$<$<BOOL:${all_defs}>:-D$<JOIN:${all_defs},$<SEMICOLON>-D>>"
        )
        set(
            all_includes_gen
            "$<$<BOOL:${all_includes}>:-I$<JOIN:${all_includes},$<SEMICOLON>-I>>"
        )
        # create agglomerated preprocessor flags
        set(
            all_pp_options
            "$<GENEX_EVAL:${all_defs_gen}$<SEMICOLON>${all_includes_gen}>"
        )
        # ensure link options fully link objects in static libs
        if(NOT WIN32)
            list(PREPEND all_link_options "-Wl,--whole-archive")
            list(APPEND all_link_options "-Wl,--no-whole-archive")
        endif()
        # pass preprocessor, compiler, and linker options in custom command
        # note: was having some issues with VERBATIM in preserving quotes
        add_custom_command(
            OUTPUT ${pkg_build_dir}/src/Makevars
            COMMAND ${CMAKE_COMMAND}
                    "-DMAKEVARS_IN=\"${pkg_build_dir}/src/Makevars.in\""
                    "-DMAKEVARS_OUT=\"${pkg_build_dir}/src/Makevars\""
                    "-DPKG_CPPFLAGS=\"${all_pp_options}\""
                    "-DPKG_CFLAGS=\"${all_compile_options}\""
                    "-DPKG_CXXFLAGS=\"${all_compile_options}\""
                    "-DPKG_LIBS=\"${all_link_options}\""
                    -P ${PROJECT_SOURCE_DIR}/cmake/oa_r_package.cmake
            # note: since configure_file() will *not* update the content of the
            # file if the input hasn't changed, even if the input is newer,
            # CMake will think the rule is out of date. we fix this by simply
            # touching the file to update the write time
            COMMAND ${CMAKE_COMMAND} -E touch "${pkg_build_dir}/src/Makevars"
            DEPENDS ${pkg_build_dir}/src/Makevars.in
            COMMENT "Configuring ${ARG_PACKAGE} Makevars"
            COMMAND_EXPAND_LISTS
        )
        # add custom target to drive the build
        add_custom_target(
            ${target}_makevars
            DEPENDS ${pkg_build_dir}/src/Makevars
        )
        # note: only run this *after* copy stage for correct deps
        add_dependencies(${target}_makevars ${clean_package}_copy)
        # add Makevars to pkg_build_files as well
        list(APPEND pkg_build_files ${pkg_build_dir}/src/Makevars)
    endif()
    # custom command for installing the DESCRIPTION file and by proxy doing the
    # actual R package installation from source. DESCRIPTION is a required file
    # which makes it very convenient as an output
    add_custom_command(
        OUTPUT ${r_prefix}/${ARG_PACKAGE}/DESCRIPTION
        COMMAND R::R CMD INSTALL ${r_options} -l ${r_prefix} ${pkg_build_dir}
        DEPENDS ${pkg_build_files}
        COMMENT "R INSTALL ${ARG_PACKAGE}"
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    # add custom target to drive the package build
    add_custom_target(
        ${target} ALL
        DEPENDS ${r_prefix}/${ARG_PACKAGE}/DESCRIPTION
    )
    add_dependencies(${target} ${clean_package}_copy)
    # ensure target also triggers Makevars configuration + is built after all
    # the other libraries that is needs to link against
    if(TARGET ${target}_makevars)
        add_dependencies(${target} ${target}_makevars)
    endif()
    if(ARG_LIBRARIES)
        add_dependencies(${target} ${ARG_LIBRARIES})
    endif()
endfunction()

# in script mode run oa_configure_r_package
if(CMAKE_SCRIPT_MODE_FILE)
    oa_configure_r_package()
endif()
