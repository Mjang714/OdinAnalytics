cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# accel_demo: demo XLL add-in using the Accel framework
if(TARGET oa_accel)
    add_library(accel_demo MODULE accel_demo.cpp)
    # need to include generated oa/version.h header
    target_include_directories(accel_demo PRIVATE ${PROJECT_BINARY_DIR}/include)
    target_link_libraries(accel_demo PRIVATE oa_accel oa_config)
    #
    # TODO:
    #
    # need some kind of launcher that will correctly set PATH and let us launch
    # Excel so that when it loads non-system XLLs it can correctly pick up the
    # relevant DLLs so people can leave the add-in registered. using /DELAYLOAD
    # with linker doesn't help as we still need explicit LoadLibraryA() calls.
    #
    set_target_properties(accel_demo PROPERTIES SUFFIX ".xll" DEBUG_POSTFIX d)
else()
    message(STATUS "Skipping accel_demo (requires Accel XLL framework)")
endif()

# axl_demo.cpp: demo XLL add-in using Accel framework and OXL API functions
if(TARGET oa_accel)
    add_library(
        axl_demo MODULE
        # XLL module
        axl_demo.cpp
        # duplicated OXL sources that could be built as a static library
        ${ODIN_SOURCE_DIR}/oxl/excel_base_funcs.cpp
        ${ODIN_SOURCE_DIR}/oxl/time_xl.cpp
        ${ODIN_SOURCE_DIR}/oxl/xl_api/cache_xl_obj.cpp
        ${ODIN_SOURCE_DIR}/oxl/xl_api/xl_array.cpp
        ${ODIN_SOURCE_DIR}/oxl/xl_api/xl_converter_funcs.cpp
        ${ODIN_SOURCE_DIR}/oxl/xl_api/xl_dictionary.cpp
        ${ODIN_SOURCE_DIR}/oxl/xl_api/xloper_converter.cpp
    )
    # need to include generated oa/version.h header
    target_include_directories(axl_demo PRIVATE ${PROJECT_BINARY_DIR}/include)
    # link against Accel, OXL API headers, and OA libraries
    # note: time_xl.h requires <boost/algorithm/string.hpp>
    target_link_libraries(
        axl_demo PRIVATE
            Boost::headers
            oa_accel
            oa_derived_time
            oa_enum_mappers
            oa_static_data_cache
            oa_time
            oxl_api
    )
    set_target_properties(axl_demo PROPERTIES SUFFIX ".xll" DEBUG_POSTFIX d)
else()
    message(STATUS "Skipping axl_demo (requires Accel XLL framework)")
endif()
