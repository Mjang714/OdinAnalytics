cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# TODO: maybe don't need extra level of directory nesting
add_library(helpers utils.cpp str_utils/str_utils.cpp)
# ensure symbols are correctly exported and resolved
if(BUILD_SHARED_LIBS)
    target_compile_definitions(helpers PRIVATE OA_BUILD_HELPERS_DLL)
endif()
