cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_config: library config helpers
# note: this is always shared so we can use dladdr() and GetModuleFileNameA()
add_library(oa_config SHARED paths.cpp)
target_include_directories(oa_config PUBLIC ${ODIN_INCLUDE_DIR})
# need Boost headers for boost::core::demangle
# TODO: may split rtti.h macros into a separate ctti.h
target_link_libraries(oa_config PRIVATE Boost::headers)
# ensure the export macro is defined during shared build + use d suffix to
# distinguish the debug library from the release library
set_target_properties(
    oa_config PROPERTIES
    DEFINE_SYMBOL OA_BUILD_CONFIG_DLL
    DEBUG_POSTFIX d
)
# on Windows need kernel32 while POSIX systems need libdl
if(WIN32)
    target_link_libraries(oa_config PRIVATE kernel32)
else()
    target_link_libraries(oa_config PRIVATE dl)
endif()
# install rule for oa_config
# TODO: remove RUNTIME spec once we have Development headers. we currently
# specify RUNTIME DESTINATION to prevent installing DLL import libs
install(
    TARGETS oa_config
    EXPORT odin_exports
    COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
)
