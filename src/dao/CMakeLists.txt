cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(dao calendar_dao.cpp)
# ensures that the specified macro is defined when building shared. note that
# if we instead use target_compile_definitions, DEFINE_SYMBOL will be
# <target>_EXPORTS by default which is unnecessary to define
set_target_properties(dao PROPERTIES DEFINE_SYMBOL OA_BUILD_DAO_DLL)
# even if not building dynamic, keeps CMake aware of dependencies
target_link_libraries(dao PRIVATE helpers PUBLIC time)
