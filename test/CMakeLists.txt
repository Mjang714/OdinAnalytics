cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# accel_demo: demo XLL add-in using the Accel framework
if(TARGET oa_accel)
    add_library(accel_demo MODULE accel_demo.cpp)
    # need to include generated oa/version.h header
    target_include_directories(accel_demo PRIVATE ${PROJECT_BINARY_DIR}/include)
    # TODO: expose an XLL function for the OA data directory
    target_link_libraries(accel_demo PRIVATE oa_accel oa_config)
    #
    # TODO:
    #
    # need some kind of launcher that will correctly set PATH and let us launch
    # Excel so that when it loads non-system XLLs it can correctly pick up the
    # relevant DLLs so people can leave the add-in registered. using /DELAYLOAD
    # with linker doesn't help as we still need explicit LoadLibraryA() calls.
    #
    set_target_properties(accel_demo PROPERTIES SUFFIX ".xll" DEBUG_POSTFIX d)
else()
    message(STATUS "Skipping accel_demo (requires Accel XLL framework)")
endif()
