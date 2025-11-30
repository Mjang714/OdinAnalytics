cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

##
# Add a CTest test for testing installation of the given component.
#
# The test name will be "oa_install_${lower_comp}_test" where ${lower_comp} is
# ${comp} but in lowercase and the installation will be done into a
# subdirectory of PROJECT_BINARY_DIR of the same name. Per-config subdirectory
# and install config will be added appropriately for multi-config generators.
#
# An extra test named "oa_install_${lower_comp}_clean" will be added as a
# cleanup step used as a test fixture to ensure clean installs.
#
# Arguments:
#   comp    Installation component name, e.g. "Runtime"
#
function(oa_install_test comp)
    # component in lowercase
    string(TOLOWER "${comp}" lower_comp)
    # target install directory. multi-config generators install into an extra
    # per-config subdirectory so we can examine the results independently
    set(install_dir ${PROJECT_BINARY_DIR}/oa_install_${lower_comp}_test)
    if(ODIN_MULTI_CONFIG)
        string(APPEND install_dir "/$<CONFIG>")
    endif()
    # add cleanup step
    add_test(
        NAME oa_install_${lower_comp}_clean
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${install_dir}"
    )
    set_tests_properties(
        oa_install_${lower_comp}_clean PROPERTIES
        # note: don't need to name fixtures the same as the associated test
        FIXTURES_SETUP oa_install_${lower_comp}_clean
    )
    # install test args. multi-config generators need the --config argument
    set(
        install_args
            --install "${PROJECT_BINARY_DIR}"
            --prefix "${install_dir}"
            --component ${comp}
    )
    if(ODIN_MULTI_CONFIG)
        list(APPEND install_args --config "$<CONFIG>")
    endif()
    # add install test
    add_test(
        NAME oa_install_${lower_comp}_test
        COMMAND ${CMAKE_COMMAND} ${install_args}
        COMMAND_EXPAND_LISTS
    )
    # require clean step to run first
    set_tests_properties(
        oa_install_${lower_comp}_test PROPERTIES
        FIXTURES_REQUIRED oa_install_${lower_comp}_clean
    )
endfunction()

# test Runtime + Development component installation
oa_install_test(Runtime)
oa_install_test(Development)
