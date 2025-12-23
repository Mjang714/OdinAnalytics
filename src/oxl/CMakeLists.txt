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

##
# Add a build run that runs oxlgen on the specified YAML file.
#
# This uses add_custom_command() to construct a build rule for the .cpp file
# specified by the oxl YAML interface files. If the OutputFile specified is
# used in a target in the current source directory, CMake provides the rule so
# that when the YAML file or header dependencies change, rebuild is automatic.
#
# Arguments:
#   yml_file        YAML interface file input for oxlgen.py
#
function(oa_yml_interface yml_file)
    # convert to absolute path + add file name for convenience
    cmake_path(ABSOLUTE_PATH yml_file OUTPUT_VARIABLE yml_path)
    cmake_path(GET yml_file FILENAME yml_file_name)
    # read OutputFile + RegFile from YAML interface
    file(STRINGS ${yml_path} cpp_file REGEX "^OutputFile[ ]*:[ ]*.+")
    file(STRINGS ${yml_path} reg_header REGEX "^RegFile[ ]*:[ ]*.+")
    # remove key prefix
    string(
        REGEX REPLACE "^OutputFile[ ]*:[ ]*([^\n]+)" "\\1"
        cpp_file "${cpp_file}"
    )
    string(
        REGEX REPLACE "^RegFile[ ]*:[ ]*([^\n]+)" "\\1"
        reg_header "${reg_header}"
    )
    # add custom build rule invoking oxlgen.py
    add_custom_command(
        OUTPUT  ${cpp_file} ${reg_header}
        COMMAND python oxlgen.py
                --oxl-root ${ODIN_SOURCE_DIR}
                -MMD -MF ${CMAKE_CURRENT_BINARY_DIR}/${yml_file_name}.d
                -o ${CMAKE_CURRENT_BINARY_DIR} ${yml_path}
        MAIN_DEPENDENCY ${yml_path}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running oxlgen.py on ${yml_file_name}"
        DEPFILE ${CMAKE_CURRENT_BINARY_DIR}/${yml_file_name}.d
    )
endfunction()

# add rules for YML interfaces
oa_yml_interface(xl_base.yml)
oa_yml_interface(xl_time.yml)

# oxl: Odin XLL add-in
if(Python3_FOUND AND XLLSDK_FOUND)
    add_library(
        oxl MODULE
            # generated headers
            autogen_excel.cpp
            auto_gen_time.cpp
            # source tree headers
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
    # add generated headers to include path
    target_include_directories(oxl PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
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
    # install PDB when building for Debug config
    # note: oxl always built shared so linker PDB file can always be used
    oa_pdb_install(oxl CONFIGURATIONS Debug COMPONENT Development)
else()
    message(STATUS "Skipping oxl (requires Python + XLL SDK)")
endif()
