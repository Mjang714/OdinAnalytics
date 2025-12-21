cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# target install directory for install tests. multi-config generators will have
# an extra per-config subdirectory so we can look at results independently
set(oa_test_install_dir "${PROJECT_BINARY_DIR}/oa_install_test")
if(ODIN_MULTI_CONFIG)
    string(APPEND oa_test_install_dir "/$<CONFIG>")
endif()

# cleanup fixture step to ensure every run gets a fresh directory
add_test(
    NAME oa_install_test_clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${oa_test_install_dir}"
)
set_tests_properties(
    oa_install_test_clean PROPERTIES
    # note: don't need to name fixture the same as the associated test
    FIXTURES_SETUP oa_install_test_clean
)

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
# require clean step to run first
set_tests_properties(
    oa_install_test PROPERTIES
    FIXTURES_REQUIRED oa_install_test_clean
)

# TODO: add external configuration test
