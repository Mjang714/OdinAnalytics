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
    # TODO: refactor XLLSDK::xlcall32 to XLLSDK::SDK later
    target_link_libraries(oa_accel PRIVATE XLLSDK::xlcall32)
else()
    message(STATUS "Skipping oa_accel (requires XLL SDK)")
endif()
