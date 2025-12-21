cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

##
# Add a CTest cleanup fixture to delete a specified directory.
#
# This creates a test using add_test() and also sets FIXTURES_SETUP.
#
# Arguments:
#   name                Name given to add_test()
#   TARGET_DIR dir      Directory to delete using cmake -E rm -rf
#
#   [FIXTURE_NAME fixture_name]
#                       Alternate test fixture name to use instead of ${name}
#
function(oa_cleanup_fixture name)
    # parse arguments
    cmake_parse_arguments(ARG "" "TARGET_DIR;FIXTURE_NAME" "" ${ARGN})
    # check for required TARGET_DIR
    if(NOT ARG_TARGET_DIR)
        message(FATAL_ERROR "TARGET_DIR argument required")
    endif()
    # if FIXTURE_NAME not provided set it to ${name}
    if(NOT ARG_FIXTURE_NAME)
        set(ARG_FIXTURE_NAME ${name})
    endif()
    # add cleanup test + register as setup fixture
    add_test(
        NAME ${name}
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${ARG_TARGET_DIR}
    )
    set_tests_properties(${name} PROPERTIES FIXTURES_SETUP ${ARG_FIXTURE_NAME})
endfunction()

# target install directory for install tests. multi-config generators will have
# an extra per-config subdirectory so we can look at results independently
set(oa_test_install_dir "${PROJECT_BINARY_DIR}/oa_install_test")
if(ODIN_MULTI_CONFIG)
    string(APPEND oa_test_install_dir "/$<CONFIG>")
endif()
# cleanup fixture step to ensure every run gets a fresh directory
oa_cleanup_fixture(oa_install_test_clean TARGET_DIR "${oa_test_install_dir}")
# install test arguments. multi-config generators need --config
set(
    install_test_args
        --install "${PROJECT_BINARY_DIR}"
        --prefix "${oa_test_install_dir}"
)
if(ODIN_MULTI_CONFIG)
    list(APPEND install_test_args --config "$<CONFIG>")
endif()
# install test
# note: assumes that both Runtime and Development are included
add_test(
    NAME oa_install_test
    COMMAND ${CMAKE_COMMAND} ${install_test_args}
    COMMAND_EXPAND_LISTS
)
# require clean step to run first + add as new fixture
set_tests_properties(
    oa_install_test PROPERTIES
    FIXTURES_SETUP oa_install_test
    FIXTURES_REQUIRED oa_install_test_clean
)

# directory for external usage test build directory
set(oa_ext_usage_test_dir "${PROJECT_BINARY_DIR}/oa_ext_usage_test")
# cleanup fixture step to ensure every run gets a fresh directory
oa_cleanup_fixture(oa_ext_usage_test_clean TARGET_DIR "${oa_ext_usage_test_dir}")

# external configuration test args. this only runs the CMake configuration step
set(ext_usage_config_test_args -S . -B "${oa_ext_usage_test_dir}")
# for Visual Studio generators we need to match the arch exactly
if(CMAKE_VS_PLATFORM_NAME)
    list(APPEND ext_usage_config_test_args -A ${CMAKE_VS_PLATFORM_NAME})
endif()
# for single-config generators we need to set CMAKE_BUILD_TYPE
if(NOT ODIN_MULTI_CONFIG)
    list(
        APPEND ext_usage_config_test_args
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    )
endif()
# add any -D values to define cache variables for the test project
list(
    APPEND ext_usage_config_test_args
    # use oa_test_install_dir as the value of ${PROJECT_NAME}_ROOT
    "-D${PROJECT_NAME}_ROOT=${oa_test_install_dir}"
    # provide CMAKE_CXX_STANDARD
    "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
    # provide target version
    # note: using ${PROJECT_VERSION} to only use major.minor.patch version
    "-DOA_TARGET_VERSION=${PROJECT_VERSION}"
)
# add external configuration test. cleanup + install step must succeed
add_test(
    NAME oa_ext_usage_config_test
    COMMAND ${CMAKE_COMMAND} ${ext_usage_config_test_args}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/oa_ext_usage_test
    COMMAND_EXPAND_LISTS
)
set_tests_properties(
    oa_ext_usage_config_test PROPERTIES
    FIXTURES_SETUP oa_ext_usage_config_test
    FIXTURES_REQUIRED "oa_ext_usage_test_clean;oa_install_test"
)

# external build test args. this only runs the CMake build step
set(ext_usage_build_test_args "${oa_ext_usage_test_dir}")
# for multi-config generators we need to use --config
if(ODIN_MULTI_CONFIG)
    list(APPEND ext_usage_build_test_args --config "$<CONFIG>")
endif()
# try to build in parallel if possible
list(APPEND ext_usage_build_test_args -j)
# add external build test (requires configuration success)
add_test(
    NAME oa_ext_usage_build_test
    COMMAND ${CMAKE_COMMAND} --build ${ext_usage_build_test_args}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/oa_ext_usage_test
    COMMAND_EXPAND_LISTS
)
set_tests_properties(
    oa_ext_usage_build_test PROPERTIES
    FIXTURES_SETUP oa_ext_usage_build_test
    FIXTURES_REQUIRED oa_ext_usage_config_test
)

# external CTest test args. this runs the CTest testing step
set(ext_usage_ctest_test_args --test-dir "${oa_ext_usage_test_dir}")
# for multi-config generators we need to use -C
if(ODIN_MULTI_CONFIG)
    list(APPEND ext_usage_ctest_test_args -C "$<CONFIG>")
endif()
# run test verbosely so we can see the command + output
list(APPEND ext_usage_ctest_test_args -V)
# add external test execution using CTest (required build success)
add_test(
    NAME oa_ext_usage_run_test
    COMMAND ${CMAKE_CTEST_COMMAND} ${ext_usage_ctest_test_args}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/oa_ext_usage_test
    COMMAND_EXPAND_LISTS
)
set_tests_properties(
    oa_ext_usage_run_test PROPERTIES
    FIXTURES_REQUIRED oa_ext_usage_build_test
)
