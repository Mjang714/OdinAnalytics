cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_unit_test: Odin master unit test runner
add_executable(
    oa_unit_test
    # broken, probably because bond math stuff is in solutions, not src
    # bond_math_test.cpp
    calendar_test.cpp
    config/paths_test.cpp
    dao/calendar_dao_test.cpp
    date_test.cpp
    derived_time/cashflow_gen/cashflow_gen_test.cpp
    derived_time/date_formula/business_date_formula_test.cpp
    derived_time/date_formula/date_formula_test.cpp
    fixed_string_test.cpp
    helpers/business_logic_check_test.cpp
    helpers/enum_test.cpp
    static_data_cache/static_data_cache_test.cpp
    string_test.cpp
    tenor_test.cpp
    time/date_adjust/adjustment_factory_test.cpp
    time/date_adjust/date_adjust_following_test.cpp
    time/date_adjust/date_adjust_modified_following.cpp
    time/date_adjust/date_adjust_plain_test.cpp
    time/date_adjust/date_adjust_preceding_test.cpp
    time/day_count/day_count_30_360_bond_basis_test.cpp
    time/day_count/day_count_30_360_e_isda_test.cpp
    time/day_count/day_count_30_e_360_eurobond_basis_test.cpp
    time/day_count/day_count_act_360_test.cpp
    time/day_count/day_count_act_365_fixed_test.cpp
    time/day_count/day_count_act_act_test.cpp
    time/day_count/day_count_factory_test.cpp
    time_enum_mappers_test.cpp
)
# add oxl tests if oxl_api is available
# note: no need to check if on Windows as oxl_api is Windows-only
if(TARGET oxl_api)
    target_sources(
        oa_unit_test PRIVATE
        oxl/handle_prefixer_test.cpp
        oxl/xl_array_test.cpp
        oxl/xl_dictionary_test.cpp
    )
endif()
# add xlcall32_stub tests if xlcall32_stub is available
if(TARGET xlcall32_stub)
    target_sources(oa_unit_test PRIVATE xlcall32_stub_test.cpp)
endif()
# MSVC emits C5264 a lot for Google Test fixtures
if(MSVC)
    target_compile_options(oa_unit_test PRIVATE /wd5264)
endif()
target_link_libraries(
    oa_unit_test
    PRIVATE
        GTest::gmock_main
        GTest::gmock
        GTest::gtest
        oa_config
        oa_dao
        oa_derived_time
        oa_helpers
        oa_static_data_cache
        oa_time
)
# link against oxl_api as necessary
if(TARGET oxl_api)
    target_link_libraries(oa_unit_test PRIVATE oxl_api)
endif()
# link against XLL SDK for xlcall32_stub tests as they include XLCALL.H
# note: Excel12[v]() symbol references typically would have been provided by
# oxl_api since it links against XLLSDK::Framework which compiles XLCALL.CPP
if(TARGET xlcall32_stub)
    target_link_libraries(oa_unit_test PRIVATE XLLSDK::SDK12)
endif()
# ensure before oa_unit_test is built that static data is correctly copied.
# although we could have hung this as a post-build command, this more correctly
# chains the dependency of oa_unit_test on the static data being located in the
# same directory as the project artifacts (for local builds)
add_dependencies(oa_unit_test oa_copy_static_data)
#
# if linking against oxl_api, which indirectly links against XLCALL32.LIB and
# therefore will require XLCALL32.DLL to be available at runtime, we must have
# the XLCALL32.DLL stub built before oa_unit_test. otherwise, oa_unit_test will
# not load correctly when gtest_discover_tests() is called and break the build.
#
# note:
#
# this issue was *not* caught on CI because currently we don't make any calls
# to XLCALL32.DLL exported functions so the linker likely just stripped out the
# unused function references as part of its optimization pass.
#
if(TARGET oxl_api)
    add_dependencies(oa_unit_test xlcall32_stub)
endif()

# enable defining OA_UNITY_ID for each agglomerated TU during unity builds
# note: disabled for now as it is unused
# oa_unity_enable_unique_id(oa_unit_test)

# discover tests for CTest (timeout increased beyond default 5 seconds for CI)
gtest_discover_tests(oa_unit_test DISCOVERY_TIMEOUT 15 PROPERTIES LABELS CXX)
