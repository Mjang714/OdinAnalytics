cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# xlcall32_stub: a stub implementation of the real Excel XLCALL32 DLL
#
# this is provided so that any targets that link against the XLL SDK directly
# or indirectly can correctly load at runtime. furthermore, any XLCALL32.DLL
# exported functions called during runtime will complete but simply return an
# error or failure value like xlRetFailed.
#
# see xlcall32_stub.cpp for more details on how the stub should be used.
#
if(XLLSDK_FOUND)
    add_library(xlcall32_stub MODULE xlcall32_stub.cpp xlcall32_stub.def)
    target_link_libraries(xlcall32_stub PRIVATE XLLSDK::SDK)
    # make output name the same as the "real" XLCALL32.DLL
    set_target_properties(
        xlcall32_stub PROPERTIES
        OUTPUT_NAME "XLCALL32"
        SUFFIX ".DLL"
        PDB_NAME "XLCALL32"
    )
else()
    message(STATUS "Skipping xlcall32_stub (requires XLL SDK)")
endif()

# add library targets
add_subdirectory(config)
add_subdirectory(dao)
add_subdirectory(derived_time)
add_subdirectory(enum_mappers)
add_subdirectory(helpers)
add_subdirectory(oxl)
add_subdirectory(static_data_cache)
add_subdirectory(time)
# add unit test target
if(GTest_FOUND AND GMock_FOUND)
    add_subdirectory(unit_test)
else()
    message(STATUS "Skipping oa_unit_test (requires GoogleTest + GoogleMock)")
endif()
