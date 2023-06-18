cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# add library targets
add_subdirectory(dao)
add_subdirectory(enum_mappers)
add_subdirectory(helpers)
# only available for Windows
if(WIN32)
    # add_subdirectory(oxl)
else()
    message(STATUS "Skipping oxl, on non-Windows platform")
endif()
add_subdirectory(static_data_cache)
add_subdirectory(time)
# add unit test target
add_subdirectory(unit_test)
