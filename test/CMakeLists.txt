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
set(oa_test_install_dir "${PROJECT_BINARY_DIR}/OA.InstallTest")
if(ODIN_MULTI_CONFIG)
    string(APPEND oa_test_install_dir "/$<CONFIG>")
endif()
# cleanup fixture step to ensure every run gets a fresh directory
oa_cleanup_fixture(OA.InstallTestClean TARGET_DIR "${oa_test_install_dir}")
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
    NAME OA.InstallTest
    COMMAND ${CMAKE_COMMAND} ${install_test_args}
    COMMAND_EXPAND_LISTS
)
# require clean step to run first + add as new fixture
set_tests_properties(
    OA.InstallTest PROPERTIES
    FIXTURES_SETUP OA.InstallTest
    FIXTURES_REQUIRED OA.InstallTestClean
)

# directory for external usage test source + build directories
set(oa_ext_usage_test_srcdir "${CMAKE_CURRENT_SOURCE_DIR}/OA.ExtUsageTest")
set(oa_ext_usage_test_bindir "${PROJECT_BINARY_DIR}/OA.ExtUsageTest")
# cleanup fixture step to ensure every run gets a fresh directory
oa_cleanup_fixture(OA.ExtUsageTestClean TARGET_DIR "${oa_ext_usage_test_bindir}")

# external configuration test args. this only runs the CMake configuration step
set(ext_usage_config_test_args -S . -B "${oa_ext_usage_test_bindir}")
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
    NAME OA.ExtUsageConfigTest
    COMMAND ${CMAKE_COMMAND} ${ext_usage_config_test_args}
    WORKING_DIRECTORY "${oa_ext_usage_test_srcdir}"
    COMMAND_EXPAND_LISTS
)
set_tests_properties(
    OA.ExtUsageConfigTest PROPERTIES
    FIXTURES_SETUP OA.ExtUsageConfigTest
    FIXTURES_REQUIRED "OA.ExtUsageTestClean;OA.InstallTest"
)

# external build test args. this only runs the CMake build step
set(ext_usage_build_test_args "${oa_ext_usage_test_bindir}")
# for multi-config generators we need to use --config
if(ODIN_MULTI_CONFIG)
    list(APPEND ext_usage_build_test_args --config "$<CONFIG>")
endif()
# try to build in parallel if possible
list(APPEND ext_usage_build_test_args -j)
# add external build test (requires configuration success)
add_test(
    NAME OA.ExtUsageBuildTest
    COMMAND ${CMAKE_COMMAND} --build ${ext_usage_build_test_args}
    WORKING_DIRECTORY "${oa_ext_usage_test_srcdir}"
    COMMAND_EXPAND_LISTS
)
set_tests_properties(
    OA.ExtUsageBuildTest PROPERTIES
    FIXTURES_SETUP OA.ExtUsageBuildTest
    FIXTURES_REQUIRED OA.ExtUsageConfigTest
)

# external CTest test args. this runs the CTest testing step
set(ext_usage_ctest_test_args --test-dir "${oa_ext_usage_test_bindir}")
# for multi-config generators we need to use -C
if(ODIN_MULTI_CONFIG)
    list(APPEND ext_usage_ctest_test_args -C "$<CONFIG>")
endif()
# run test verbosely so we can see the command + output
list(APPEND ext_usage_ctest_test_args -V)
# add external test execution using CTest (required build success)
add_test(
    NAME OA.ExtUsageRunTest
    COMMAND ${CMAKE_CTEST_COMMAND} ${ext_usage_ctest_test_args}
    WORKING_DIRECTORY "${oa_ext_usage_test_srcdir}"
    COMMAND_EXPAND_LISTS
)
set_tests_properties(
    OA.ExtUsageRunTest PROPERTIES
    FIXTURES_REQUIRED OA.ExtUsageBuildTest
)

# oa_config_py tests
if(TARGET oa_config_py)
    oa_unittest_discover_tests(oa_config_py_tests TESTS oa_config_test.py)
    add_dependencies(oa_config_py_tests oa_config_py)
endif()

# oa_r_testutils: OA R test helpers package
# note: this is "built" by direct installation via R CMD INSTALL which copies
# from the oa.testutils directory. however, we avoid running this all the time
# with a custom target by making using a custom command for the installed
# DESCRIPTION file as we cannot mark a directory as a generated source
if(R_FOUND)
    # custom command for installing the DESCRIPTION file and by proxy doing the
    # actual R package installation from source. we have to specify a file, not
    # a directory, and the DESCRIPTION file is always required
    add_custom_command(
        OUTPUT ${PROJECT_BINARY_DIR}/R/oa.testutils/DESCRIPTION
        # must ensure directory is created
        # TODO: this should be run as a custom target to prevent races
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/R
        # note: unnecessary for source builds but important for binary builds
        # as R CMD INSTALL will pollute the source tree
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/oa.testutils
                ${CMAKE_CURRENT_BINARY_DIR}/oa.testutils
        # install from build directory for a clean source tree
        COMMAND R::R CMD INSTALL -l ${PROJECT_BINARY_DIR}/R
                ${CMAKE_CURRENT_BINARY_DIR}/oa.testutils
        # note: could glob for convenience but IDE builds won't automatically
        # reconfigure to pick up added files since the CMakeLists.txt would not
        # have changed. in general globbing is discouraged for this reason
        DEPENDS oa.testutils/DESCRIPTION
                oa.testutils/LICENSE
                oa.testutils/NAMESPACE
                oa.testutils/R/sys.R
                oa.testutils/R/tu.R
        COMMENT "R INSTALL oa.testutils"
        VERBATIM
    )
    # add custom target to drive the build
    add_custom_target(
        oa_r_testutils ALL
        DEPENDS ${PROJECT_BINARY_DIR}/R/oa.testutils/DESCRIPTION
    )
else()
    message(STATUS "Skipping oa_r_testutils (requires R)")
endif()

# oa_config_r tests
# note: only this one basic test script (not much we can test anyways)
if(TARGET oa_config_r)
    add_test(
        NAME oa_config_r_test
        COMMAND R::Rscript "${CMAKE_CURRENT_SOURCE_DIR}/oa_config_test.R"
        WORKING_DIRECTORY "${ODIN_BUILD_DIR_GENEX}"
    )
    set_tests_properties(
        oa_config_r_test PROPERTIES
        # ensure installed oa.testutils package can be loaded with library()
        ENVIRONMENT "R_LIBS=${PROJECT_BINARY_DIR}/R"
        LABELS R
    )
endif()
