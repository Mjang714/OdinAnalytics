cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_enum_mappers
# TODO: need better descriptive comment
add_library(oa_enum_mappers time_enum_mappers.cpp)
add_library(OdinAnalytics::oa_enum_mappers ALIAS oa_enum_mappers)
target_include_directories(
    oa_enum_mappers PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
    # note: not installable just yet
    $<INSTALL_INTERFACE:include>
)
# set export macro for shared build + use d suffix for debug builds
set_target_properties(
    oa_enum_mappers PROPERTIES
    DEFINE_SYMBOL OA_BUILD_ENUM_MAPPERS_DLL
    DEBUG_POSTFIX d
)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(oa_enum_mappers INTERFACE OA_ENUM_MAPPERS_DLL)
endif()
# install rule for oa_enum_mappers
# TODO: add ARCHIVE COMPONENT Development once we have development headers
install(
    TARGETS oa_enum_mappers
    EXPORT odin_exports
    LIBRARY COMPONENT Runtime
    RUNTIME COMPONENT Runtime
)
