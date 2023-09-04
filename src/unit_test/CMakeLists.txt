cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# strongly recommend name change to something like oa_test
add_executable(
    unit_test
    # should put this test file in unit_test
    ../time/day_count/day_count_act_365_fixed_test.cpp
    # broken, probably because bond math stuff is in solutions, not src
    # bond_math_test.cpp
    calendar_test.cpp
    date_test.cpp
    dao/calendar_dao_test.cpp
    date_test.cpp
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
# MSVC emits C5264 a lot for Google Test fixtures
if(MSVC)
    target_compile_options(unit_test PRIVATE /wd5264)
endif()
target_link_libraries(
    unit_test
    PRIVATE GTest::gtest_main dao enum_mappers helpers static_data_cache time
)
