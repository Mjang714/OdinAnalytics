cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_dao: dao library
# TODO: need better description comment here
add_library(oa_dao calendar_dao.cpp)
target_include_directories(
    oa_dao PUBLIC
    $<BUILD_INTERFACE:${ODIN_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ODIN_SOURCE_DIR}>
    # note: not installable just yet
    $<INSTALL_INTERFACE:include>
)
# set export macro for shared build + use d library suffix for debug builds
set_target_properties(
    oa_dao PROPERTIES
    DEFINE_SYMBOL OA_BUILD_DAO_DLL
    DEBUG_POSTFIX d
)
# set fine-grained visibility macro
if(BUILD_SHARED_LIBS)
    target_compile_definitions(oa_dao INTERFACE OA_DAO_DLL)
endif()
# even if not building dynamic, keeps CMake aware of dependencies
# TODO: since Boost::headers is for oa/rtti.h maybe we want an INTERFACE target
target_link_libraries(oa_dao PUBLIC Boost::headers oa_helpers oa_time)
# install rule for oa_dao
install(
    TARGETS oa_dao
    EXPORT odin_exports
    ARCHIVE COMPONENT Development
    LIBRARY COMPONENT Runtime
    RUNTIME COMPONENT Runtime
)
# install PDB when building for Debug config
oa_pdb_install(oa_dao CONFIGURATIONS Debug COMPONENT Development)
