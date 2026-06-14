cmake_minimum_required(VERSION 3.22)

##
# oa_r_discover_tests.cmake
#
# Enables CTest registration of R test scripts using oa.testutils helpers.
#

include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/oa_require_nonempty.cmake)

##
# Implementation function for registering R test scripts with CTest.
#
#
# The following variables must be defined:
#
#   R_SCRIPT_EXECUTABLE     Path to the Rscript R interpreter frontend
#   OUTPUT_DIR              Output directory for the .cmake test file
#   OUTPUT_FILE             Filename of the .cmake test file to write
#
#   TEST_FILE               Relative path to the .R script, expected to be
#                           relative to CMAKE_CURRENT_SOURCE_DIR
#
#   R_ROOT                  Top-level R package install directory in the build
#                           tree, e.g. ${PROJECT_BINARY_DIR}/R
#
#   BINARY_DIR              Top-level binary directory, typically
#                           PROJECT_BINARY_DIR with a per-config subdirectory
#                           when using a multi-config generator
#
#   CURRENT_SOURCE_DIR      Current source directory being processed, typically
#                           the last value of CMAKE_CURRENT_SOURCE_DIR
#
# The following variables are optional:
#
#   R_LIBS                  Semicolon-separated list of directories
#   LD_LIBRARY_PATH         Semicolon-separated list of directories
#   WORKING_DIRECTORY       Working directory the test will run in
#
# Both R_LIBS and LD_LIBRARY_PATH will be prepended to the environment values.
#
function(oa_r_discover_tests_impl)
    # check required variables
    set(
        required_vars
        R_SCRIPT_EXECUTABLE
        OUTPUT_DIR OUTPUT_FILE TEST_FILE R_ROOT BINARY_DIR CURRENT_SOURCE_DIR
    )
    foreach(var ${required_vars})
        if(NOT ${var})
            message(FATAL_ERROR "${var} required")
        endif()
    endforeach()
    # prepend R_LIBS, LD_LIBRARY_PATH to environment and defaults
    list(APPEND R_LIBS "${R_ROOT}" $ENV{R_LIBS})
    list(APPEND LD_LIBRARY_PATH "${BINARY_DIR}" $ENV{LD_LIBRARY_PATH})
    # if not on Windows replace semiclon with colons
    if(NOT WIN32)
        list(JOIN R_LIBS ":" R_LIBS)
        list(JOIN LD_LIBRARY_PATH ":" LD_LIBRARY_PATH)
    endif()
    # output file name
    set(output_path "${OUTPUT_DIR}/${OUTPUT_FILE}")
    # make absolute relative to the current source directory
    cmake_path(
        ABSOLUTE_PATH TEST_FILE
        BASE_DIRECTORY "${CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE test_path
    )
    # list tests in script
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env
                "R_LIBS=${R_LIBS}" "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" --
                "${R_SCRIPT_EXECUTABLE}" "${test_path}" --list-tests
        WORKING_DIRECTORY "${WORKING_DIRECTORY}"
        # note: R's message() function writes to stderr by default
        OUTPUT_VARIABLE test_list
        ERROR_VARIABLE test_list
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    # convert into list
    string(REPLACE "\n" ";" test_list "${test_list}")
    # build list of tests
    # note: see oa_unittest_discover_tests_impl() for a discussion on why the
    # given commands behave differently in CTest
    foreach(test ${test_list})
        string(
            APPEND test_content
"# ${test}\n"
"add_test(\n"
"    ${test}\n"
# first change working directory
"    \"${CMAKE_COMMAND}\" -E chdir \"${WORKING_DIRECTORY}\"\n"
# run in modified environment
"    \"${CMAKE_COMMAND}\" -E env\n"
"        \"R_LIBS=${R_LIBS}\"\n"
"        \"LD_LIBRARY_PATH=${LD_LIBRARY_PATH}\"\n"
"        \"--\"\n"
"        \"${R_SCRIPT_EXECUTABLE}\" \"${test_path}\" -t ${test}\n"
")\n"
# set test properties. see oa_unittest_discover_tests_impl() for why the
# set_tests_properties() command is support but set_property() is not
"set_tests_properties(${test} PROPERTIES LABELS R)\n"
        )
    endforeach()
    # write into file
    file(WRITE "${output_path}" "${test_content}")
endfunction()

##
# Add an R script test target for registering the tests with CTest.
#
# This target allows CTest registration of R scripts that use the oa.testutils
# test helpers. The scripts run their tests using oa.tu_main() and are able to
# list their test function names if passed the --list-tests command-line option
# and can run a single test if given the -t <test> option.
#
# Each test target can be configured using set_property(TARGET...) or
# set_target_properties() to modify the runtime R_LIBS, LD_LIBRARY_PATH, etc.
# Below is the list of properties that can be set in the test environment:
#
#   R_LIBS                  Semicolon-separated list of directories to prepend
#                           to R_LIBS for locating R packages. By default, the
#                           top-level binary directory's R directory is added,
#                           and generator expressions can be used.
#
#   LD_LIBRARY_PATH         Semicolon-separated list of directories to prepend
#                           to LD_LIBRARY_PATH to influence where libraries are
#                           loaded from. By default, the top-level binary
#                           directory, with per-config subdirectory, and the
#                           current working directory, are prepended. Generator
#                           expressions can be specified.
#
#   WORKING_DIRECTORY       Working directory the test will run in. By default,
#                           the working directory is CMAKE_CURRENT_SOURCE_DIR.
#                           Generator expressions can be specified.
#
# Each of the R test function names in the named vector of tests passed to
# oa.tu_main() will be registered as a CTest test of the exact same name and
# automatically have the "R" label associated with the test.
#
# This function is very similar to oa_unittest_discover_tests() conceptually,
# only differing in some the minor details of running scripts and variables. It
# also requires that the FindR.cmake find module has successfully found R.
#
# Arguments:
#   target          Test target name
#   TESTS file      R script with tests. Must be able to accept a --list-tests
#                   command-line option and print out a list of its test names.
#
function(oa_r_discover_tests target)
    # parse arguments
    cmake_parse_arguments(ARG "" "TESTS" "" ${ARGN})
    if(NOT ARG_TESTS)
        message(FATAL_ERROR "TESTS required")
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "unparsed arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    # indicate if using multi-config generator
    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # generator expressions for properties
    set(r_libs "$<TARGET_PROPERTY:${target},R_LIBS>")
    set(ld_libs "$<TARGET_PROPERTY:${target},LD_LIBRARY_PATH>")
    set(working_dir "$<TARGET_PROPERTY:${target},WORKING_DIRECTORY>")
    # set working directory via generator expression
    set(
        working_dir_expr
        "$<IF:$<BOOL:${working_dir}>,${working_dir},${CMAKE_CURRENT_SOURCE_DIR}>"
    )
    # custom target to run this file in script mode for test listing
    add_custom_target(
        ${target} ALL
        COMMAND ${CMAKE_COMMAND}
                -DR_SCRIPT_EXECUTABLE=${R_SCRIPT_EXECUTABLE}
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
                -DOUTPUT_FILE=${target}-$<IF:${is_multi_config},$<CONFIG>,impl>.cmake
                -DTEST_FILE=${ARG_TESTS}
                -DR_ROOT=${PROJECT_BINARY_DIR}/R
                -DBINARY_DIR=${PROJECT_BINARY_DIR}$<${is_multi_config}:/$<CONFIG>>
                -DCURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                # R_LIBS, LD_LIBRARY_PATH, WORKING_DIRECTORY setting for tests
                # note: evaluate generator expression for implementation script
                -DR_LIBS=$<$<BOOL:${r_libs}>:$<GENEX_EVAL:${r_libs}>>
                -DLD_LIBRARY_PATH=$<$<BOOL:${ld_libs}>:$<GENEX_EVAL:${ld_libs}>>
                -DWORKING_DIRECTORY=$<GENEX_EVAL:${working_dir_expr}>
                -P ${PROJECT_SOURCE_DIR}/cmake/oa_r_discover_tests.cmake
        COMMENT "Collecting ${target} R tests"
        VERBATIM
        # note: no COMMAND_EXPAND_LISTS since that would break list values
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    # write top-level CTest script to include
    # note: nearly the same logic as in oa_unittest_discover_tests()
    set(test_include "${CMAKE_CURRENT_BINARY_DIR}/${target}.cmake")
    if(is_multi_config)
        file(
            WRITE "${test_include}"
"if(NOT CTEST_CONFIGURATION_TYPE)\n"
"    message(FATAL_ERROR \"Missing test configuration (use -C <config>)\")\n"
"endif()\n"
"include(\"${CMAKE_CURRENT_BINARY_DIR}/${target}-\${CTEST_CONFIGURATION_TYPE}.cmake\")\n"
        )
    else()
        file(
            WRITE "${test_include}"
"include(\"${CMAKE_CURRENT_BINARY_DIR}/${target}-impl.cmake\")\n"
        )
    endif()
    # ensure CTest includes the test_include tests
    set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${test_include}")
endfunction()

# run implementation in script mode
if(CMAKE_SCRIPT_MODE_FILE)
    oa_r_discover_tests_impl()
endif()
