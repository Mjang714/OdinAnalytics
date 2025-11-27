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
# ensures that the specified macro is defined when building shared
set_target_properties(
    oa_static_data_cache PROPERTIES
    DEFINE_SYMBOL OA_BUILD_STATIC_DATA_CACHE_DLL
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
