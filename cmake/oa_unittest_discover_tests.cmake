cmake_minimum_required(VERSION 3.22)

##
# oa_unittest_discover_tests.cmake
#
# Enables CTest registration of unittest tests using oa_testutils.test_main.
#

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
# Add a Python unittest test target.
#
# This target will be built after any dependent targets and will initially be
# run with -l, --list-tests to get the list of test cases. Then,
function(oa_unittest_discover_tests_impl)
    # check required variables
    oa_require_nonempty(Python3_EXECUTABLE)
    oa_require_nonempty(OUTPUT_DIR)
    oa_require_nonempty(OUTPUT_FILE)
    oa_require_nonempty(TEST_FILE)
    oa_require_nonempty(BINARY_DIR)
    oa_require_nonempty(CURRENT_SOURCE_DIR)
    oa_require_nonempty(WORKING_DIRECTORY)
    # prepend PATH, PYTHON_PATH to environment values
    list(APPEND RUNTIME_PATH $ENV{PATH})
    list(APPEND PYTHON_PATH $ENV{PYTHONPATH})
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
    # note: can't use set_property(TEST ...) since test is not immediately
    # known to CTest even though we are having CTest process the file
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
"        \"PYTHONPATH=${PYTHON_PATH}\"\n"
"        \"--\"\n"
"        \"${Python3_EXECUTABLE}\" \"${test_path}\" -t ${test}\n"
")\n"
        )
    endforeach()
    # write into file
    file(WRITE "${output_path}" "${test_content}")
endfunction()

##
# Add a Python unittest test target that runs the tests
#
#
# This target will be built after any dependent targets and will initially be
# run with -l, --list-tests to get the list of test cases. Then,
function(oa_unittest_discover_tests target)
    # parse arguments
    cmake_parse_arguments(ARG "" "TESTS" "" ${ARGN})
    if(NOT ARG_TESTS)
        message(FATAL_ERROR "TESTS required")
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
