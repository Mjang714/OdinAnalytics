/**
 * @file accel_demo.cpp
 * @author Derek Huang
 * @brief C++ source for an Accel XLL framework demo XLL
 * @copyright MIT License
 */

#include <string>

#include "oa/accel/addin.h"
#include "oa/version.h"

namespace {

// basic add-in
OA_ACCEL_ADDIN_INSTANCE()
  .name("Accel Demo " + std::string{ODIN_VERSION});

}  // namespace
