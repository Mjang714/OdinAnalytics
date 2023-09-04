cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# TODO: maybe don't need extra level of directory nesting
add_library(helpers utils.cpp str_utils/str_utils.cpp)
# ensures that the specified macro is defined when building shared
set_target_properties(helpers PROPERTIES DEFINE_SYMBOL OA_BUILD_HELPERS_DLL)
