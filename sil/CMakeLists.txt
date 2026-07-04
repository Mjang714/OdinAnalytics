cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# oa_sil: SWIG Interface Library
add_library(oa_sil INTERFACE)
target_include_directories(
    oa_sil INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sil>
    $<INSTALL_INTERFACE:share/OA/sil>
)
install(TARGETS oa_sil EXPORT odin_exports)
# ensure .i files are installed to share/OA/sil
install(DIRECTORY oa DESTINATION share/OA/sil COMPONENT Development)
