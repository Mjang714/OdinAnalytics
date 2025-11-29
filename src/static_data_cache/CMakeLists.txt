cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_static_data_cache
# TODO: need better descriptive comment
add_library(oa_static_data_cache calendar_cache.cpp)
add_library(OdinAnalytics::oa_static_data_cache ALIAS oa_static_data_cache)
target_include_directories(
    oa_static_data_cache PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
    # note: not installable just yet
    $<INSTALL_INTERFACE:include>
)
# set export macro for shared build + use d suffix for debug builds
set_target_properties(
    oa_static_data_cache PROPERTIES
    DEFINE_SYMBOL OA_BUILD_STATIC_DATA_CACHE_DLL
    DEBUG_POSTFIX d
)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(
        oa_static_data_cache INTERFACE
        OA_STATIC_DATA_CACHE_DLL
    )
endif()
# even if not building dynamic, keeps CMake aware of dependencies
target_link_libraries(oa_static_data_cache PUBLIC Boost::headers oa_dao oa_time)
# install rule for oa_static_data_cache
# TODO: add ARCHIVE COMPONENT Development once we have development headers
install(
    TARGETS oa_static_data_cache
    EXPORT odin_exports
    LIBRARY COMPONENT Runtime
    RUNTIME COMPONENT Runtime
)
