cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(enum_mappers time_enum_mappers.cpp)
# ensures that the specified macro is defined when building shared
set_target_properties(
    enum_mappers PROPERTIES
    DEFINE_SYMBOL OA_BUILD_ENUM_MAPPERS_DLL
)
