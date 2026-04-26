cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_config: library config helpers
# note: this is always shared so we can use dladdr() and GetModuleFileNameA()
add_library(oa_config SHARED paths.cpp)
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
# on Windows need kernel32 while POSIX systems need libdl
if(WIN32)
    target_link_libraries(oa_config PRIVATE kernel32)
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
if(WIN32)
    add_executable(oa_dumpver dumpver.cpp)
    # need to be able to include oa/version.h
    target_include_directories(oa_dumpver PRIVATE ${PROJECT_BINARY_DIR}/include)
    # need to link against Win32 version.lib
    target_link_libraries(oa_dumpver PRIVATE version)
    # use snake-case style for executables
    set_target_properties(oa_dumpver PROPERTIES OUTPUT_NAME "oa-dumpver")
    oa_embed_version_info(oa_dumpver DESCRIPTION "OA Version Dumper")
    # simple -h, --help tests
    add_test(NAME oa_dumpver_h COMMAND oa_dumpver -h)
    add_test(NAME oa_dumpver_help COMMAND oa_dumpver --help)
    set_tests_properties(
        oa_dumpver_h
        oa_dumpver_help PROPERTIES
        PASS_REGULAR_EXPRESSION "Usage: oa-dumpver"
    )
else()
    message(STATUS "Skipping oa_dumpver (Windows-only)")
endif()
