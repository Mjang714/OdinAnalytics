cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(enum_mappers time_enum_mappers.cpp)
# ensure symbols are correctly exported and resolved
if(BUILD_SHARED_LIBS)
    target_compile_definitions(enum_mappers PRIVATE OA_BUILD_ENUM_MAPPERS_DLL)
endif()
