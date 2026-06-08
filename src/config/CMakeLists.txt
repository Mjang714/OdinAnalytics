cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_config: library config helpers
# note: this is always shared so we can use dladdr() and GetModuleFileNameA()
add_library(oa_config SHARED paths.cpp version_info.cpp)
target_include_directories(
    oa_config PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
# need Boost headers for boost::core::demangle
# TODO: may split rtti.h macros into a separate ctti.h
# TODO: drop use of boost::core::demangle and use abi::__cxa_demangle directly
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
oa_check_version_info(oa_config)

# oa_dumpver: prints version info embedded in OA binaries
# note: although provided for testing purposes this could be useful in general
add_executable(oa_dumpver dumpver.cpp)
# need to be able to include oa/version.h
target_include_directories(oa_dumpver PRIVATE ${PROJECT_BINARY_DIR}/include)
target_link_libraries(oa_dumpver PRIVATE oa_config)
# use snake-case style for executables
set_target_properties(oa_dumpver PROPERTIES OUTPUT_NAME "oa-dumpver")
oa_embed_version_info(oa_dumpver DESCRIPTION "OA Version Dumper")
oa_check_version_info(oa_dumpver)
# install rule for oa_dumpver + ensure relocatable on install by setting RPATH
install(TARGETS oa_dumpver COMPONENT Runtime)
set_target_properties(oa_dumpver PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
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
# test error reported if run with no arguments
add_test(NAME oa_dumpver COMMAND oa_dumpver)
set_tests_properties(
    oa_dumpver PROPERTIES
    PASS_REGULAR_EXPRESSION "Missing required value for FILE argument"
)
# check that oa-dumpver doesn't crash on incorrect file types by feeding its
# own C++ source file as the input and checking the exception message
add_test(
    NAME oa_dumpver_cpp_input
    COMMAND oa_dumpver "${CMAKE_CURRENT_SOURCE_DIR}/dumpver.cpp"
)
if(WIN32)
    set_tests_properties(
        oa_dumpver_cpp_input PROPERTIES
        PASS_REGULAR_EXPRESSION "image file did not contain a resource section"
    )
else()
    set_tests_properties(
        oa_dumpver_cpp_input PROPERTIES
        PASS_REGULAR_EXPRESSION "input stream doesn't point to an ELF file"
    )
endif()

# oa_config_py: SWIG module with oa_config.py module wrapper
if(SWIG_python_FOUND AND Python3_FOUND AND Python3_Development_FOUND)
    oa_swig_module(oa_config_py LANGUAGE python MODULE module.i CXX DOXYGEN)
    target_link_libraries(oa_config_py PRIVATE Python3::Module oa_config oa_sil)
else()
    message(STATUS "Skipping oa_config_py (requires SWIG + Python libraries)")
endif()

# oa_config_r: SWIG module with oa_config_r.R module wrapper
#
# note:
#
# for exposition purposes. it works, but SWIG's R support is abysmal, so
# out-of-the-box the .R wrapper is useless. loading the oa_config_r DSO with
# dyn.load() and then using .Call() works, but is of course clunky. also, the
# version_info class doesn't get wrapped correctly, and std::filesystem::path
# never has a setClass() call so R has no idea how to treat it. building R C++
# extensions also won't work on Windows with MSVC since the R toolchain uses a
# MinGW GCC toolchain which has a different ABI.
#
if(SWIG_r_FOUND AND R_FOUND AND R_Development_FOUND)
    oa_swig_module(oa_config_r LANGUAGE r MODULE module.i CXX DOXYGEN)
    # note: output name explicitly set to avoid conflict with importing
    # oa_config in Python as the DSO has priority over the .py file
    set_target_properties(oa_config_r PROPERTIES OUTPUT_NAME oa_config_r)
    target_link_libraries(oa_config_r PRIVATE R::R oa_config oa_sil)
else()
    message(STATUS "Skipping oa_config_r (requires SWIG + R libraries)")
endif()
