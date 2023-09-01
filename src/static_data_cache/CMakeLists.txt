cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(static_data_cache calendar_cache.cpp)
# ensure symbols are correctly exported and resolved
if(BUILD_SHARED_LIBS)
    target_compile_definitions(
        static_data_cache
        PRIVATE OA_BUILD_STATIC_DATA_CACHE_DLL
    )
    target_link_libraries(static_data_cache PUBLIC dao time)
endif()
