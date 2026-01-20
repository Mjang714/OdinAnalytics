/**
 * @file accel_demo.cpp
 * @author Derek Huang
 * @brief C++ source for an Accel XLL framework demo XLL
 * @copyright MIT License
 */

#include <string>

#include "oa/accel/addin.h"
#include "oa/accel/oper12.h"
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
  .help("Return the path to the OA static data directory");

/**
 * Return the maximum of the two values.
 */
OA_XLL_EXPORT(double) OaMax(double a, double b)
{
  return (a > b) ? a : b;
}

OA_ACCEL_EXPORT_FUNC(OaMax)
  .category("OA Math")
  .help("Return the maximum of the two values")
  .arg("a", "First value")
  .arg("b", "Second value");

/**
 * Convert all the characters in the string to upper case.
 */
OA_XLL_EXPORT(xloper12*) OaToUpper(const char* in) OA_ACCEL_SAFE()
{
  std::string str{in};
  // change to uppercase as appropriate
  for (auto& c : str)
    if (c >= 'a' && c <= 'z')
      c -= ('a' - 'A');
  // provide back to Excel
  OA_ACCEL_SAFE_RETURN(accel::oper12{str}.release());
}

OA_ACCEL_EXPORT_FUNC(OaToUpper)
  .category("OA String")
  .help("Convert lowercase characters in the string to upper case")
  .arg("s", "Input string");

// TODO: add naive matmul

}  // namespace oa
