cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_unit_test: Odin master unit test runner
add_executable(
    oa_unit_test
    # FIXME: should put this test file in unit_test
    ../time/day_count/day_count_act_365_fixed_test.cpp
    # broken, probably because bond math stuff is in solutions, not src
    # bond_math_test.cpp
    calendar_test.cpp
    config/paths_test.cpp
    dao/calendar_dao_test.cpp
    date_test.cpp
    derived_time/cashflow_gen/cashflow_gen_test.cpp
    derived_time/date_formula/date_formula_test.cpp
    helpers/business_logic_check_test.cpp
    helpers/enum_test.cpp
    static_data_cache/static_data_cache_test.cpp
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
    time/day_count/day_count_act_act_test.cpp
    time/day_count/day_count_factory_test.cpp
)
# add oxl tests if oxl_api is available
# note: no need to check if on Windows as oxl_api is Windows-only
if(TARGET oxl_api)
    target_sources(oa_unit_test PRIVATE oxl/handle_prefixer_test.cpp)
endif()
# MSVC emits C5264 a lot for Google Test fixtures
if(MSVC)
    target_compile_options(oa_unit_test PRIVATE /wd5264)
endif()
target_link_libraries(
    oa_unit_test
    PRIVATE
        GTest::gtest_main
        oa_config
        oa_dao
        oa_derived_time
        oa_enum_mappers
        oa_helpers
        oa_static_data_cache
        oa_time
)
# link against oxl_api as necessary
if(TARGET oxl_api)
    target_link_libraries(oa_unit_test PRIVATE oxl_api)
endif()

# discover tests for CTest
gtest_discover_tests(
    oa_unit_test
    # ensure that OdinBaseDir is correctly set up
    PROPERTIES ENVIRONMENT "OdinBaseDir=${PROJECT_SOURCE_DIR}"
)
