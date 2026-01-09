cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# xlcall32_stub: a stub implementation of the real Excel XLCALL32 DLL
#
# this is provided so that any targets that link against the XLL SDK or
# oa_accel (which links against the XLL SDK implicitly) can correctly load at
# runtime. furthermore, any XLCALL32.DLL exported function calls will complete,
# but simply return a invalid/error failure like xlfetFailed.
#
# see xlcall32_stub.cpp for more details on how the stub should be used.
#
if(XLLSDK_FOUND)
    add_library(xlcall32_stub MODULE xlcall32_stub.cpp xlcall32_stub.def)
    # TODO: refactor XLLSDK::xlcall32 to XLLSDK::SDK later
    target_link_libraries(xlcall32_stub PRIVATE XLLSDK::xlcall32)
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
add_subdirectory(accel)
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
