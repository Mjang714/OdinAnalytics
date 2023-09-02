cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(dao calendar_dao.cpp)
# ensure symbols are correctly exported and resolved
if(BUILD_SHARED_LIBS)
    # note: technically only need the macro on Windows, but if we decide to use
    # -fvisibility=hidden, then we only need update dllexport.h
    target_compile_definitions(dao PRIVATE OA_DLL OA_BUILD_DAO_DLL)
    target_link_libraries(dao PRIVATE helpers PUBLIC time)
endif()
