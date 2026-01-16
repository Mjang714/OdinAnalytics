cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_accel: Excel C++ SDK for XLLs
if(XLLSDK_FOUND)
    add_library(oa_accel STATIC addin.cpp mref12.cpp oper12.cpp xl_ops.cpp)
    target_include_directories(
        oa_accel PUBLIC
        $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    # XLL SDK dependency is private
    target_link_libraries(oa_accel PRIVATE XLLSDK::SDK12)
    # use d postfix for debug builds like other libraries
    set_target_properties(oa_accel PROPERTIES DEBUG_POSTFIX d)
    # install rule for Accel library (using static defaults)
    install(TARGETS oa_accel EXPORT odin_exports ARCHIVE COMPONENT Development)
    # install PDB for Debug config for downstream
    oa_pdb_install(oa_accel CONFIGURATIONS Debug COMPONENT Development)
else()
    message(STATUS "Skipping oa_accel (requires XLL SDK)")
endif()
