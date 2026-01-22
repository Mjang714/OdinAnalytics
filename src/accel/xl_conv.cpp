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

#include <concepts>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "oa/accel/enums.h"

namespace oa {
namespace accel {

////////////////////////////////////////////////////////////////////////////////
// scalar converters                                                          //
////////////////////////////////////////////////////////////////////////////////

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

bool as_bool(const xloper12& op)
{
  switch (op.xltype) {
  case xltypeBool:
    return !!op.val.xbool;
  case xltypeInt:
    return !!op.val.w;
  default:
    throw std::runtime_error{
      std::string{"cannot convert XLOPER12 of type "} +
      // note: require cast to avoid narrowing for C++17 enum class list-init
      to_string(xltype{static_cast<int>(op.xltype)}) + " to bool"
    };
  }
}

int as_int(const xloper12& op)
{
  switch (op.xltype) {
  case xltypeBool:
    return !!op.val.xbool;
  case xltypeInt:
    return op.val.w;
  default:
    throw std::runtime_error{
      std::string{"cannot convert XLOPER12 of type "} +
      // note: require cast to avoid narrowing for C++17 enum class list-init
      to_string(xltype{static_cast<int>(op.xltype)}) + " to int"
    };
  }
}

////////////////////////////////////////////////////////////////////////////////
// multi_conv_options                                                         //
////////////////////////////////////////////////////////////////////////////////

multi_conv_options&
multi_conv_options::strict(bool v) noexcept
{
  strict_ = v;
  return *this;
}

bool
multi_conv_options::strict() const noexcept
{
  return strict_;
}

multi_conv_options&
multi_conv_options::vector(bool v) noexcept
{
  vector_ = v;
  row_vector_ = (row_vector_ && v);
  col_vector_ = (col_vector_ && v);
  return *this;
}

bool
multi_conv_options::vector() const noexcept
{
  return vector_;
}

multi_conv_options&
multi_conv_options::row_vector(bool v) noexcept
{
  row_vector_ = vector_ = v;
  return *this;
}

bool
multi_conv_options::row_vector() const noexcept
{
  return row_vector_;
}

multi_conv_options::self&
multi_conv_options::col_vector(bool v) noexcept
{
  col_vector_ = vector_ = v;
  return *this;
}

bool
multi_conv_options::col_vector() const noexcept
{
  return col_vector_;
}

////////////////////////////////////////////////////////////////////////////////
// multi converters                                                           //
////////////////////////////////////////////////////////////////////////////////

namespace {

/**
 * Helper function to fill a floating-point buffer with `xltypeMulti` values.
 *
 * This implements the non-template `as()` buffer overloads.
 *
 * @tparam T Floating-point type
 *
 * @param out Output buffer
 * @param op `XLOPER12` to convert
 * @param ops Additional `xltypeMulti` conversion options
 */
template <std::floating_point T>
void as_impl(T* out, const xloper12& op, const multi_conv_options& opts = {})
{
  // lambda for exception for strict types
  auto raise = [type = op.xltype]
  {
    throw std::runtime_error{
      std::string{"cannot fill floating-point buffer from XLOPER12 of type "} +
      // note: require cast to avoid narrowing for C++17 enum class list-init
      to_string(xltype{static_cast<int>(type)})
    };
  };
  // switch on type
  switch (op.xltype) {
  // support converting a single value
  case xltypeInt:
    if (opts.strict())
      raise();
    out[0] = static_cast<T>(op.val.w);
    return;
  case xltypeNum:
    if (opts.strict())
      raise();
    out[0] = static_cast<T>(op.val.num);
    return;
  // convert the entire array
  case xltypeMulti: {
    // dimensions
    auto n_rows = op.val.array.rows;
    auto n_cols = op.val.array.columns;
    // dimension requirements
    if (opts.vector() && (n_rows != 1) && (n_cols != 1))
      throw std::runtime_error{"xltypeMulti is not a row or column vector"};
    if (opts.row_vector() && (n_rows != 1))
      throw std::runtime_error{
        "xltypeMulti has " + std::to_string(n_rows) +
        " rows instead of the required 1 row"
      };
    if (opts.col_vector() && (n_cols != 1))
      throw std::runtime_error{
        "xltypeMulti has " + std::to_string(n_cols) +
        " columns instead of the required 1 column"
      };
    // note: current as_double() contains all the conversion lotic
    for (auto i = 0; i < op.val.array.rows * op.val.array.columns; i++)
      out[i] = static_cast<T>(as_double(op.val.array.lparray[i]));
    return;
  }
  default:
    raise();
  }
}

}  // namespace

void as(double* out, const xloper12& op, const multi_conv_options& opts)
{
  as_impl(out, op, opts);
}

void as(float* out, const xloper12& op, const multi_conv_options& opts)
{
  as_impl(out, op, opts);
}

namespace {

/**
 * Helper function to obtain a float vector from `xltypeMulti` values.
 *
 * This implements the `as_<float type>_vector()` overloads.
 *
 * @tparam T Floating-point type
 * @tparam A Allocator
 *
 * @param op `XLOPER12` to convert
 * @param opts Additional `xltypeMulti` conversion options
 */
template <std::floating_point T, typename A = std::allocator<T>>
auto as_vector(const xloper12& op, const multi_conv_options& opts = {})
{
  // get array size (assume 1 if not array)
  auto size = [&op]() -> std::size_t
  {
    switch (op.xltype) {
    case xltypeMulti:
      return op.val.array.rows * op.val.array.columns;
    default:
      return 1;
    }
  }();
  // allocate vector
  // note: as() will throw appropriately so we don't need error checking here
  std::vector<T, A> out(size);
  // fill + return
  as_impl(out.data(), op, opts);
  return out;
}

}  // namespace

std::vector<double>
as_double_vector(const xloper12& op, const multi_conv_options& opts)
{
  return as_vector<double>(op, opts);
}

std::vector<float>
as_float_vector(const xloper12& op, const multi_conv_options& opts)
{
  return as_vector<float>(op, opts);
}

}  // namespace accel
}  // namespace oa
