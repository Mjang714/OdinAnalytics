cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_config: library config helpers
# note: this is always shared so we can use dladdr() and GetModuleFileNameA()
add_library(oa_config SHARED paths.cpp)
target_include_directories(oa_config PUBLIC ${ODIN_INCLUDE_DIR})
set_target_properties(oa_config PROPERTIES DEFINE_SYMBOL OA_BUILD_CONFIG_DLL)
# on Windows need kernel32 while POSIX systems need libdl
if(WIN32)
    target_link_libraries(oa_config PRIVATE kernel32)
else()
    target_link_libraries(oa_config PRIVATE dl)
endif()
