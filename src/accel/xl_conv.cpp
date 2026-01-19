/**
 * @file xl_conv.cpp
 * @author Derek Huang
 * @brief C++ source for `XLCALL.H` type converters
 * @copyright MIT License
 */

#include "oa/accel/xl_conv.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "oa/accel/enums.h"

namespace oa {
namespace accel {

double as_double(const xloper12& op)
{
  switch (op.xltype) {
  case xltypeNum:
    return op.val.num;
  case xltypeBool:
    return op.val.xbool;
  case xltypeInt:
    return op.val.w;
  default:
    throw std::runtime_error{
      std::string{"cannot convert XLOPER12 of type "} +
      // note: require cast to avoid narrowing for C++17 enum class list-init
      to_string(xltype{static_cast<int>(op.xltype)}) + " to double"
    };
  }
}

std::string as_string(const xloper12& op)
{
  switch (op.xltype) {
  case xltypeNum:
    return std::to_string(op.val.num);
  case xltypeStr: {
    // note: size of string is first character
    std::string s(static_cast<unsigned>(op.val.str[0]), '\0');
    // narrow + return
    for (auto i = 0u; i < s.size(); i++)
      s[i] = std::wcout.narrow(op.val.str[i + 1], '?');
    return s;
  }
  case xltypeBool:
    return (!!op.val.xbool) ? "true" : "false";
  // note: essentially copied from operator<<
  case xltypeErr:
    switch (op.val.err) {
    case xlerrNull:
      return "#NULL!";
    case xlerrDiv0:
      return "#DIV/0!";
    case xlerrValue:
      return "#VALUE!";
    case xlerrRef:
      return "#REF!";
    case xlerrName:
      return "#NAME?";
    case xlerrNum:
      return "#NUM!";
    case xlerrNA:
      return "#N/A";
    case xlerrGettingData:
      return "#WAIT!";
    }
  case xltypeInt:
    return std::to_string(op.val.w);
  default:
    throw std::runtime_error{
      std::string{"cannot convert XLOPER12 of type "} +
      // note: require cast to avoid narrowing for C++17 enum class list-init
      to_string(xltype{static_cast<int>(op.xltype)}) + " to std::string"
    };
  }
}

std::wstring as_wstring(const xloper12& op)
{
  switch (op.xltype) {
  case xltypeNum:
    return std::to_wstring(op.val.num);
  case xltypeStr: {
    // note: size of string is first character
    std::wstring s(static_cast<unsigned>(op.val.str[0]), L'\0');
    // copy + return
    std::memcpy(s.data(), &op.val.str[1], sizeof(XCHAR) * s.size());
    return s;
  }
  case xltypeBool:
    return (!!op.val.xbool) ? L"true" : L"false";
  // note: this is copied from operator<<
  case xltypeErr:
    switch (op.val.err) {
    case xlerrNull:
      return L"#NULL!";
    case xlerrDiv0:
      return L"#DIV/0!";
    case xlerrValue:
      return L"#VALUE!";
    case xlerrRef:
      return L"#REF!";
    case xlerrName:
      return L"#NAME?";
    case xlerrNum:
      return L"#NUM!";
    case xlerrNA:
      return L"#N/A";
    case xlerrGettingData:
      return L"#WAIT!";
    }
  case xltypeInt:
    return std::to_wstring(op.val.w);
  default:
    throw std::runtime_error{
      std::string{"cannot convert XLOPER12 of type "} +
      // note: require cast to avoid narrowing for C++17 enum class list-init
      to_string(xltype{static_cast<int>(op.xltype)}) + " to std::wstring"
    };
  }
}

std::wstring_view as_wstring_view(const xloper12& op)
{
  switch (op.xltype) {
  case xltypeStr:
    // note: size of string is first character
    return {&op.val.str[1], static_cast<unsigned>(op.val.str[0])};
  default:
    throw std::runtime_error{
      std::string{"cannot get std::wstring_view from XLOPER12 of type "} +
      // note: require cast to avoid narrowing for C++17 enum class list-init
      to_string(xltype{static_cast<int>(op.xltype)}) + " (must be xltypeStr)"
    };
  }
}

}  // namespace accel
}  // namespace oa
