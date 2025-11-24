cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# add library targets
add_subdirectory(dao)
add_subdirectory(derived_time)
add_subdirectory(enum_mappers)
add_subdirectory(helpers)
add_subdirectory(oxl)
add_subdirectory(static_data_cache)
add_subdirectory(time)
# add unit test target
if(GTest_FOUND)
    add_subdirectory(unit_test)
else()
    message(STATUS "Skipping oa_unit_test (requires GoogleTest)")
endif()
