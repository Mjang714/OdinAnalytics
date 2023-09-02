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
# ensure symbols are correctly exported and resolved
if(BUILD_SHARED_LIBS)
    target_compile_definitions(time PRIVATE OA_DLL OA_BUILD_TIME_DLL)
    # note: only uses some inline helpers so this step doesn't do anything
    target_link_libraries(time PRIVATE helpers)
endif()
