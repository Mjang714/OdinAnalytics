cmake_minimum_required(VERSION 3.22)

##
# oa_unittest_discover_tests.cmake
#
# Enables CTest registration of unittest tests using oa_testutils.test_main.
#

include_guard(GLOBAL)

##
# Helper function to require that a variable is nonempty.
#
# Arguments:
#   var         Variable to check
#
function(oa_require_nonempty var)
    if(NOT ${var})
        message(FATAL_ERROR "${var} required")
    endif()
endfunction()

##
# Implementation function for registering unittest tests with CTest.
#
# This is run when the CMake module is invoked in script mode and will invoke
# the given test script with the --list-tests option and write a .cmake file
# in OUTPUT_DIR, typically CMAKE_CURRENT_BINARY_DIR, that has old-style
# add_test() invocations that run the actual test case with the -t option.
#
# The following variables must be defined:
#
#   TARGET_NAME             Name of the associated CMake target
#   Python3_EXECUTABLE      Path to the Python 3 interpreter used
#   OUTPUT_DIR              Output directory for the .cmake test file
#   OUTPUT_FILE             Filename of the .cmake test file to write
#
#   TEST_FILE               Relative path to the .py unittest script, expected
#                           to be relative to CURRENT_SOURCE_DIR
#
#   BINARY_DIR              Top-level binary directory, typically
#                           PROJECT_BINARY_DIR with a per-config subdirectory
#                           in the case of a multi-config generator
#
#   CURRENT_SOURCE_DIR      Current source directory being processed, typically
#                           the last CMAKE_CURRENT_SOURCE_DIR
#
# The folowing variables are optional:
#
#   PYTHON_PATH             Semicolon-separated list of directories to prepend
#                           to PYTHONPATH when running the tests
#
#   WORKING_DIRECTORY       Working directory the test will run in
#
function(oa_unittest_discover_tests_impl)
    # check required variables
    oa_require_nonempty(TARGET_NAME)
    oa_require_nonempty(Python3_EXECUTABLE)
    oa_require_nonempty(OUTPUT_DIR)
    oa_require_nonempty(OUTPUT_FILE)
    oa_require_nonempty(TEST_FILE)
    oa_require_nonempty(BINARY_DIR)
    oa_require_nonempty(CURRENT_SOURCE_DIR)
    oa_require_nonempty(WORKING_DIRECTORY)
    # prepend PATH, PYTHON_PATH to environment values and defaults
    # note: RUNTIME_PATH currently used
    list(APPEND RUNTIME_PATH "${BINARY_DIR}" $ENV{PATH})
    list(APPEND PYTHON_PATH "." "${BINARY_DIR}" $ENV{PYTHONPATH})
    # if not on Windows replace semicolon with colons for PATH, PYTHONPATH
    if(NOT WIN32)
        list(JOIN RUNTIME_PATH ":" RUNTIME_PATH)
        list(JOIN PYTHON_PATH ":" PYTHON_PATH)
    endif()
    # output file name
    set(output_path "${OUTPUT_DIR}/${OUTPUT_FILE}")
    # get absolute path relative to current source directory
    cmake_path(
        ABSOLUTE_PATH TEST_FILE
        BASE_DIRECTORY "${CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE test_path
    )
    # list tests in script
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env "PYTHONPATH=${PYTHON_PATH}" --
                "${Python3_EXECUTABLE}" "${test_path}" --list-tests
        WORKING_DIRECTORY "${WORKING_DIRECTORY}"
        OUTPUT_VARIABLE test_list
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    # convert into list
    string(REPLACE "\n" ";" test_list "${test_list}")
    # build up list of tests
    # note: must use old add_test() format
    foreach(test ${test_list})
        set(test_name "${TARGET_NAME}.${test}")
        string(
            APPEND test_content
"# ${test_name}\n"
"add_test(\n"
"    ${test_name}\n"
# first change working directory
"    \"${CMAKE_COMMAND}\" -E chdir \"${WORKING_DIRECTORY}\"\n"
# run in modified environment
"    \"${CMAKE_COMMAND}\" -E env\n"
"        \"PYTHONPATH=${PYTHON_PATH}\"\n"
"        \"--\"\n"
"        \"${Python3_EXECUTABLE}\" \"${test_path}\" -t ${test}\n"
")\n"
# although not documented, CTest does support set_tests_properties(). see the
# discourse.cmake.org thread about differences in set_property() vs.
# set_tests_properties() for discovered GTest tests. note also that in CMake's
# Source/CTest/cmCTestTestHandler.cxx the only supported commands are:
#
#   add_test()
#   discover_tests()
#   subdirs()
#   add_subdirectory()
#   set_tests_properties()
#   set_directory_properties()
#
"set_tests_properties(${test_name} PROPERTIES LABELS Python)\n"
        )
    endforeach()
    # write into file
    file(WRITE "${output_path}" "${test_content}")
endfunction()

##
# Add a Python unittest test target for registering the tests with CTest.
#
# This target allows registration of Python scripts that run unittest test
# cases, can list their <class>.<function> test cases if given the --list-tests
# command-line option, and can run a single test if given the -t <test> option,
# with CTest. For example, any unittest script that uses the
# oa_testutils.test_main() satisfies this interface.
#
# Each test target can be configured using set_property(TARGET ...) or
# set_target_properties() to modify the runtime PYTHONPATH, etc. Below is the
# list of properties that can be set to influence the test environment:
#
#   PYTHON_PATH             Semicolon-separated list of directories to prepend
#                           to PYTHONPATH for imports. By default, the top-level
#                           binary directory, with per-config subdirectory, and
#                           the current working directory, will already be
#                           prepended. Generator expressions can be specified.
#
#   WORKING_DIRECTORY       Working directory the test will run in. By default,
#                           the working directory is CMAKE_CURRENT_SOURCE_DIR.
#                           Generator exprssions can be specified.
#
# Each <class>.<function> unittest test will be registered as a CTest test with
# the name "<target>.<class>.<function>", where <target> is the target name. In
# addition, each test will automatically have the "Python" label associated
# with the test, allowing CTest filtering by label.
#
# Arguments:
#   target          Test target name
#   TESTS file      Python file with unittest tests. Must be able to accept a
#                   --list-tests command-line option and print out a list of
#                   its <class>.<function> test names, one per line.
#
function(oa_unittest_discover_tests target)
    # parse arguments
    cmake_parse_arguments(ARG "" "TESTS" "" ${ARGN})
    if(NOT ARG_TESTS)
        message(FATAL_ERROR "TESTS required")
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "unparsed arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    # indicate if multi-config generator is being used
    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # generator expressions for properties
    set(pythonpath "$<TARGET_PROPERTY:${target},PYTHON_PATH>")
    set(working_dir "$<TARGET_PROPERTY:${target},WORKING_DIRECTORY>")
    # working directory generator expression
    set(
        working_dir_expr
        "$<IF:$<BOOL:${working_dir}>,${working_dir},${CMAKE_CURRENT_SOURCE_DIR}>"
    )
    # custom target to run this file in script mode for listing tests
    add_custom_target(
        ${target} ALL
        COMMAND ${CMAKE_COMMAND}
                -DTARGET_NAME=${target}
                -DPython3_EXECUTABLE=${Python3_EXECUTABLE}
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
                -DOUTPUT_FILE=${target}-$<IF:${is_multi_config},$<CONFIG>,impl>.cmake
                -DTEST_FILE=${ARG_TESTS}
                -DBINARY_DIR=${PROJECT_BINARY_DIR}$<${is_multi_config}:/$<CONFIG>>
                -DCURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                # PYTHONPATH setting for test
                # note: need to force generator expression evaluation
                -DPYTHON_PATH=$<$<BOOL:${pythonpath}>:$<GENEX_EVAL:${pythonpath}>>
                # working directory setting for tests
                # note: need to force generator expression evaluation
                -DWORKING_DIRECTORY=$<GENEX_EVAL:${working_dir_expr}>
                -P ${PROJECT_SOURCE_DIR}/cmake/oa_unittest_discover_tests.cmake
        COMMENT "Collecting ${target} unittest tests"
        VERBATIM
        # note: no COMMAND_EXPAND_LISTS since that would break PYTHON_PATH list
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    # write top-level CTest script to include
    set(test_include "${CMAKE_CURRENT_BINARY_DIR}/${target}.cmake")
    if(is_multi_config)
        file(
            WRITE "${test_include}"
"if(NOT CTEST_CONFIGURATION_TYPE)\n"
"    message(FATAL_ERROR \"Missing test configuration (use -C <config>)\")\n"
"endif()\n"
"include(\"${CMAKE_CURRENT_BINARY_DIR}/${target}-\${CTEST_CONFIGURATION_TYPE}.cmake\")\n"
        )
    # for single-config generators we don't need use CTEST_CONFIGURATION_TYPE
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
    oa_unittest_discover_tests_impl()
endif()
