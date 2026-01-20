/**
 * @file accel_demo.cpp
 * @author Derek Huang
 * @brief C++ source for an Accel XLL framework demo XLL
 * @copyright MIT License
 */

#include <string>

#include "oa/accel/addin.h"
#include "oa/config/paths.h"
#include "oa/version.h"

namespace oa {

// basic add-in
OA_ACCEL_ADDIN_INSTANCE()
  .name("Accel Demo " + std::string{ODIN_VERSION});

/**
 * Return the location of the OA static data directory.
 */
OA_XLL_EXPORT(const char*) OaDataDir()
{
  // thread-safe since C++11
  static auto path = oa::config::data_dir().string();
  return path.c_str();
}

OA_ACCEL_EXPORT_FUNC(OaDataDir)
  .category("OA Config")
  .help("Return the path to the OA static data directory")
  .volatile_(true);

}  // namespace oa
