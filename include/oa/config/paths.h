/**
 * @file config/paths.h
 * @author Derek Huang
 * @brief C++ header for library path helpers
 * @copyright MIT License
 */

#ifndef OA_CONFIG_PATHS_H_
#define OA_CONFIG_PATHS_H_

#include <filesystem>

#include "oa/config/dllexport.h"

namespace oa {
namespace config {

/**
 * Return the location of the config shared library.
 */
OA_CONFIG_API
std::filesystem::path library_path();

/**
 * Return the location of the config shared library's directory.
 *
 * Since all Odin libraries are packaged in the same build/install flat layout
 * this can be used to obtain the location of the rest of the libraries too.
 *
 * @note On Windows static libraries are installed in `lib` while the actual
 *  DLLs are installed in `bin`. Since `oa_config` is always built shared, if
 *  building Odin libraries as static on Windows, `library_dir()` will return
 *  the location of DLLs, *not* of static libraries.
 */
OA_CONFIG_API
std::filesystem::path library_dir();

/**
 * Return the location of the Odin shared data directory.
 *
 * For example, if `/usr/local` is the Odin installation root and libraries are
 * installed in `/usr/local/lib`, this function returns the path corresponding
 * to the directory `/usr/local/share/OA` if it exists.
 *
 * If the corresponding `share/OA` subdirectory doesn't exist, then the path
 * returned is the same as the one returned by `library_dir()`. This behavior
 * is solely to facilitate local testing as the build artifact directory will
 * contain the requisite shared data subdirectories.
 */
OA_CONFIG_API
std::filesystem::path data_dir();

// TODO: can add specific function for calendars directory

}  // namespace config
}  // namespace oa

#endif  // OA_CONFIG_PATHS_H_
