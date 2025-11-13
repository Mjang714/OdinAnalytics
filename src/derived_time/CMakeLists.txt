cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(derived_time date_formula/date_formula.cpp)
# ensures that the specified macro is defined when building shared
set_target_properties(
    derived_time PROPERTIES
        DEFINE_SYMBOL OA_BUILD_DERIVED_TIME_DLL
)
# even if not building dynamic, keeps CMake aware of dependencies
target_link_libraries(
    derived_time
        PRIVATE enum_mappers helpers static_data_cache
        PUBLIC time
)
