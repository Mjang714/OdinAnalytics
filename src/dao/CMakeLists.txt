cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_dao: dao library
# TODO: need better description comment here
add_library(oa_dao calendar_dao.cpp)
add_library(OdinAnalytics::oa_dao ALIAS oa_dao)
target_include_directories(
    oa_dao PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
    # note: not installable just yet
    $<INSTALL_INTERFACE:include>
)
# ensures that the specified macro is defined when building shared. note that
# if we instead use target_compile_definitions, DEFINE_SYMBOL will be
# <target>_EXPORTS by default which is unnecessary to define
set_target_properties(oa_dao PROPERTIES DEFINE_SYMBOL OA_BUILD_DAO_DLL)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(oa_dao INTERFACE OA_DAO_DLL)
endif()
# even if not building dynamic, keeps CMake aware of dependencies
target_link_libraries(oa_dao PUBLIC oa_helpers oa_time)
