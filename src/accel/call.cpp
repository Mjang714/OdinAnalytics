/**
 * @file call.cpp
 * @author Derek Huang
 * @brief C++ source for functions corresponding to `Excel12()` calls
 * @copyright MIT License
 */

#include "oa/accel/call.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include "oa/accel/oper12.h"

namespace oa {
namespace accel {

std::uintptr_t window()
{
  oper12 res;
  Excel12(xlGetHwnd, res.value(), 0);
  return static_cast<std::uintptr_t>(res.value()->val.w);
}

void alert(std::string_view str)
{
  oper12 res;
  oper12 message{str};
  // note: return value ignored since we know it is always true
  Excel12(xlcAlert, res.value(), 1, message.value());
}

bool alert(std::string_view str, alert_type type)
{
  oper12 res;
  oper12 message{str};
  oper12 style{static_cast<int>(type)};
  Excel12(xlcAlert, res.value(), 2, message.value(), style.value());
  return !!res.value()->val.xbool;
}

}  // namespace accel
}  // namespace oa
