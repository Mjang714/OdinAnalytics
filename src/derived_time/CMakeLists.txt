cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_derived_time: Odin derived time library
add_library(oa_derived_time date_formula/date_formula.cpp)
add_library(OdinAnalytics::oa_derived_time ALIAS oa_derived_time)
target_include_directories(
    oa_derived_time PUBLIC
    $<BUILD_INTERFACE:${ODIN_INSTALL_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
    # note: not installable yet
    $<INSTALL_INTERFACE:include>
)
# ensures that the specified macro is defined when building shared
set_target_properties(
    oa_derived_time PROPERTIES
    DEFINE_SYMBOL OA_BUILD_DERIVED_TIME_DLL
)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(oa_derived_time INTERFACE OA_DERIVED_TIME_DLL)
endif()
target_link_libraries(
    oa_derived_time
        PRIVATE oa_enum_mappers oa_helpers oa_static_data_cache
        PUBLIC oa_time
)
