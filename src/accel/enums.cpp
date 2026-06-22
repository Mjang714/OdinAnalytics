/**
 * @file enums.cpp
 * @author Derek Huang
 * @brief C++ source for Excel type, error, return enumerations + helpers
 * @copyright MIT License
 */

#include "oa/accel/enums.h"

#include <ostream>

namespace oa {
namespace accel {

std::ostream& operator<<(std::ostream& out, xltype type)
{
  return out << to_string(type);
}

std::ostream& operator<<(std::ostream& out, xlerr err)
{
  return out << to_string(err);
}

}  // namespace accel
}  // namespace oa
