cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(
    time
    calendar.cpp
    date.cpp
    tenor.cpp
    date_adjust/adjustment_factory.cpp
    date_adjust/date_adjust_following.cpp
    date_adjust/date_adjust_modified_following.cpp
    date_adjust/date_adjust_preceding.cpp
    day_count/day_counter_factory.cpp
    day_count/day_count_30_360e_isda.cpp
    day_count/day_count_30_e_360_eurobond_basis.cpp
    day_count/day_count_30_360_bond_basis.cpp
    day_count/day_count_act_360.cpp
    day_count/day_count_act_365_fixed.cpp
    day_count/day_count_act_act.cpp
)
