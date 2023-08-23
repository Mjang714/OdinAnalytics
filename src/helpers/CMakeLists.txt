cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# TODO: maybe don't need extra level of directory nesting
add_library(helpers utils.cpp str_utils/str_utils.cpp)
