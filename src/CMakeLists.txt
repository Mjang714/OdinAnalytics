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

# oa_benchmark_main: Google benchmark main() stub
#
# this is provided to avoid issues with the upstream Ubuntu 22.04 package
# libbenchmark-dev 1.6.1-1 which provides a libbenchmark_main.a with an older,
# incompatible LTO version. since LTO doesn't really matter for a main() stub,
# we simply provide our own version to avoid any LTO incompatibilities.
#
if(benchmark_FOUND)
    add_library(oa_benchmark_main STATIC benchmark_main.cpp)
    # note: although oa_benchmark_main only requires PRIVATE linkage to build,
    # any consumer will of course need to link against the benchmark target
    # itself, so we use PUBLIC to ensure usage requirements are propagated
    target_link_libraries(oa_benchmark_main PUBLIC benchmark::benchmark)
else()
    message(STATUS "Skipping oa_benchmark_main (requires benchmark)")
endif()

# isdigit_bench: oa::is_digit() vs. std::isdigit() microbenchmark
# note: not added as a CTest test since benchmarking programs are not tests and
# should be run isolated to ensure accurate benchmarking results
if(benchmark_FOUND)
    add_executable(isdigit_bench isdigit_bench.cpp)
    target_link_libraries(isdigit_bench PRIVATE oa_benchmark_main oa_core)
else()
    message(STATUS "Skipping isdigit_bench (requires benchmark)")
endif()
