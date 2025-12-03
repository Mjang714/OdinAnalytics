cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_helpers: miscellanous Odin utils
# TODO: maybe don't need extra level of directory nesting
add_library(oa_helpers utils.cpp str_utils/str_utils.cpp)
add_library(OdinAnalytics::oa_helpers ALIAS oa_helpers)
target_include_directories(
    oa_helpers PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
)
# ensures that the specified macro is defined when building shared
set_target_properties(oa_helpers PROPERTIES DEFINE_SYMBOL OA_BUILD_HELPERS_DLL)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(oa_helpers INTERFACE OA_HELPERS_DLL)
endif()
# depends on Magic Enum
target_link_libraries(oa_helpers PUBLIC magic_enum::magic_enum)
