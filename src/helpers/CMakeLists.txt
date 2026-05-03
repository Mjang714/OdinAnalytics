cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_helpers: miscellanous Odin utils
# TODO: maybe don't need extra level of directory nesting
add_library(oa_helpers utils.cpp str_utils/str_utils.cpp)
target_include_directories(
    oa_helpers PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
)
# set export macro for shared build + use d suffix for debug builds
set_target_properties(
    oa_helpers PROPERTIES
    DEFINE_SYMBOL OA_BUILD_HELPERS_DLL
    DEBUG_POSTFIX d
)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(oa_helpers INTERFACE OA_HELPERS_DLL)
endif()
# depends on Magic Enum
target_link_libraries(oa_helpers PUBLIC magic_enum::magic_enum)
# install rule for oa_helpers
install(
    TARGETS oa_helpers
    EXPORT odin_exports
    ARCHIVE COMPONENT Development
    LIBRARY COMPONENT Runtime
    RUNTIME COMPONENT Runtime
)
# install PDB when building for Debug config
oa_pdb_install(oa_helpers CONFIGURATIONS Debug COMPONENT Development)
