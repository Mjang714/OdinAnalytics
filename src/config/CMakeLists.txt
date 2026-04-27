cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_config: library config helpers
# note: this is always shared so we can use dladdr() and GetModuleFileNameA()
add_library(oa_config SHARED paths.cpp version_info.cpp)
target_include_directories(oa_config PUBLIC ${ODIN_INCLUDE_DIR})
# need Boost headers for boost::core::demangle
# TODO: may split rtti.h macros into a separate ctti.h
target_link_libraries(oa_config PRIVATE Boost::headers)
# ensure the export macro is defined during shared build + use d suffix to
# distinguish the debug library from the release library
set_target_properties(
    oa_config PROPERTIES
    DEFINE_SYMBOL OA_BUILD_CONFIG_DLL
    DEBUG_POSTFIX d
)
# on Windows need kernel32 + version while POSIX systems need libdl
if(WIN32)
    target_link_libraries(oa_config PRIVATE kernel32 version)
else()
    target_link_libraries(oa_config PRIVATE dl)
endif()
# install rule for oa_config
install(
    TARGETS oa_config
    EXPORT odin_exports
    ARCHIVE COMPONENT Development
    LIBRARY COMPONENT Runtime
    RUNTIME COMPONENT Runtime
)
# install PDB when building shared for Debug config
# note: oa_config always built shared so linker PDB file can always be used
oa_pdb_install(oa_config CONFIGURATIONS Debug COMPONENT Development)
oa_embed_version_info(oa_config DESCRIPTION "OA Configuration Library")

# oa_dumpver: prints version info embedded in OA binaries
# note: although provided for testing purposes this could be useful in general
add_executable(oa_dumpver dumpver.cpp)
# need to be able to include oa/version.h
target_include_directories(oa_dumpver PRIVATE ${PROJECT_BINARY_DIR}/include)
target_link_libraries(oa_dumpver PRIVATE oa_config)
# use snake-case style for executables
set_target_properties(oa_dumpver PROPERTIES OUTPUT_NAME "oa-dumpver")
oa_embed_version_info(oa_dumpver DESCRIPTION "OA Version Dumper")
# -h, --help tests
add_test(NAME oa_dumpver_h COMMAND oa_dumpver -h)
add_test(NAME oa_dumpver_help COMMAND oa_dumpver --help)
set_tests_properties(
    oa_dumpver_h
    oa_dumpver_help PROPERTIES
    PASS_REGULAR_EXPRESSION "Usage: oa-dumpver"
)
# -v, --version tests
add_test(NAME oa_dumpver_v COMMAND oa_dumpver -v)
add_test(NAME oa_dumpver_version COMMAND oa_dumpver --version)
set_tests_properties(
    oa_dumpver_v
    oa_dumpver_version PROPERTIES
    PASS_REGULAR_EXPRESSION "${ODIN_VERSION}"
)

# function to construct a PASS_REGULAR_EXPRESSION for a oa_dumpver test
# TODO: should break this out for other targets later
function(oa_dumpver_test_regex target outvar)
    cmake_parse_arguments(ARG "" "DESCRIPTION" "" ${ARGN})
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
        "description:[ ]+${ARG_DESCRIPTION}\n"
        "version number:[ ]+${hex_version}\n"
        "version string:[ ]+${ODIN_VERSION}\n"
    )
    # write to outvar
    set(${outvar} "${test_regex}" PARENT_SCOPE)
endfunction()

# version dump test for oa_config
add_test(
    NAME oa_config_dumpver_test
    COMMAND oa_dumpver $<TARGET_FILE:oa_config>
)
oa_dumpver_test_regex(
    oa_config oa_config_dumpver_regex
    DESCRIPTION "OA Configuration Library"
)
set_tests_properties(
    oa_config_dumpver_test PROPERTIES
    PASS_REGULAR_EXPRESSION "${oa_config_dumpver_regex}"
)
# version dump test for oa_dumpver itself
add_test(
    NAME oa_dumpver_dumpver_test
    COMMAND oa_dumpver $<TARGET_FILE:oa_dumpver>
)
oa_dumpver_test_regex(
    oa_dumpver oa_dumpver_dumpver_regex
    DESCRIPTION "OA Version Dumper"
)
set_tests_properties(
    oa_dumpver_dumpver_test PROPERTIES
    PASS_REGULAR_EXPRESSION "${oa_dumpver_dumpver_regex}"
)
