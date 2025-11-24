cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_time: Odin time library
add_library(
    oa_time
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
target_include_directories(
    oa_time PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
    # note: not installable just yet
    $<INSTALL_INTERFACE:include>
)
# ensures that the specified macro is defined when building shared
set_target_properties(oa_time PROPERTIES DEFINE_SYMBOL OA_BUILD_TIME_DLL)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(oa_time INTERFACE OA_TIME_DLL)
endif()
# link against dependencies
target_link_libraries(oa_time PUBLIC oa_helpers PRIVATE Boost::headers)
