/**
 * @file oper12_row_view.cpp
 * @author Derek Huang
 * @brief C++ source for a view type over a row of an `xltypeMulti` operand
 * @copyright MIT License
 */

#include "oa/accel/oper12_row_view.h"

// note: must complete the xloper12 type for pointer arithmetic
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <cstddef>

#include "oa/accel/oper12_view.h"

namespace oa {
namespace accel {

oper12_row_view::oper12_row_view(const xloper12* buf, std::size_t size) noexcept
  : buf_{buf}, size_{size}
{}

oper12_view
oper12_row_view::operator[](std::size_t i) const noexcept
{
  return buf_ + i;
}

std::size_t
oper12_row_view::size() const noexcept
{
  return size_;
}

}  // namespace accel
}  // namespace oa
