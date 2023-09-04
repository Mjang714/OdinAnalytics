cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(static_data_cache calendar_cache.cpp)
# ensures that the specified macro is defined when building shared
set_target_properties(
    static_data_cache PROPERTIES
    DEFINE_SYMBOL OA_BUILD_STATIC_DATA_CACHE_DLL
)
# even if not building dynamic, keeps CMake aware of dependencies
target_link_libraries(static_data_cache PUBLIC dao time)
