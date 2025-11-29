cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oxl_api: Odin XLL C++ headers
# TODO: need to decide if this should be a "real" library or not. some of the
# oxl sources (xl_api directory) should be compiled here instead
if(XLLSDK_FOUND)
    add_library(oxl_api INTERFACE)
    add_library(OdinAnalytics::oxl_api ALIAS oxl_api)
    target_include_directories(
        oxl_api INTERFACE
        $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
        # note: not installable just yet
        $<INSTALL_INTERFACE:include>
    )
    # note: since xlcall.h and framewrk.h show up in some headers
    target_link_libraries(oxl_api INTERFACE XLLSDK::frmwrk32 XLLSDK::xlcall32)
else()
    message(STATUS "Skipping oxl_api (requires XLL SDK)")
endif()

# oxl: Odin XLL add-in
# TODO: generated code should be generated as part of build system
if(XLLSDK_FOUND)
    add_library(
        oxl MODULE
            auto_gen/autogen_excel.cpp
            auto_gen/auto_gen_time.cpp
            excel_base_funcs.cpp
            excel_functions.cpp
            time_xl.cpp
            xl_api/cache_xl_obj.cpp
            xl_api/excel_base.cpp
            xl_api/xl_array.cpp
            xl_api/xl_converter_funcs.cpp
            xl_api/xl_dictionary.cpp
            xl_api/xloper_converter.cpp
    )
    # make the extension .xll + enable d suffix for debug builds
    set_target_properties(oxl PROPERTIES SUFFIX .xll DEBUG_POSTFIX d)
    target_link_libraries(
        oxl PRIVATE
            XLLSDK::frmwrk32
            XLLSDK::xlcall32
            oa_dao
            oa_derived_time
            oa_enum_mappers
            oa_helpers
            oa_static_data_cache
            oa_time
            oxl_api
    )
    # install rule for oxl
    install(TARGETS oxl EXPORT odin_exports DESTINATION bin COMPONENT Runtime)
else()
    message(STATUS "Skipping oxl (requires XLL SDK)")
endif()
