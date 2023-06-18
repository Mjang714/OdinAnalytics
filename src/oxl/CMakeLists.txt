cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(
    oxl
    SHARED
        auto_gen/autogen_excel.cpp
        auto_gen/auto_gen_time.cpp
        excel_base_funcs.cpp
        time_xl.cpp
        xl_api/cache_xl_obj.cpp
        xl_api/excel_base.cpp
        xl_api/xloper_converter.cpp
        xl_api/xl_array.cpp
        xl_api/xl_converter_funcs.cpp
        xl_api/xl_dictionary.cpp
)
# make the extension .xll
set_target_properties(oxl PROPERTIES SUFFIX .xll)
# note: xlcall32, frmwrk32 are provided by the Excel SDK. frmwrk32 has to be
# built manually however if Excel SDK is downloaded from Microsoft web site
target_link_libraries(
    oxl
    PRIVATE dao enum_mappers frmwrk32 helpers static_data_cache time xlcall32
)
