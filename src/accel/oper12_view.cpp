/**
 * @file oper12_view.cpp
 * @author Derek Huang
 * @brief C++ source for a view type over the Excel 12 fundamental data type
 * @copyright MIT License
 */

#include "oa/accel/oper12_view.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <cstddef>
#include <optional>

#include "oa/accel/enums.h"
#include "oa/accel/xl_ops.h"

namespace oa {
namespace accel {

oper12_view::oper12_view() noexcept : oper12_view(nullptr) {}

oper12_view::oper12_view(const xloper12* value) noexcept : value_{value} {}

bool
oper12_view::operator!() const noexcept
{
  return !value_;
}

xltype
oper12_view::type() const noexcept
{
  // cast to satsify C++17 list-init
  return xltype{static_cast<int>(value_->xltype)};
}

std::optional<xlerr>
oper12_view::error() const noexcept
{
  if (type() == xltype::err)
    return xlerr{value_->val.err};
  else
    return {};
}

oper12_view
oper12_view::operator()(std::size_t i) const noexcept
{
  return &value_->val.array.lparray[i];
}

oper12_view
oper12_view::operator()(std::size_t i, std::size_t j) const noexcept
{
  return &value_->val.array.lparray[i * value_->val.array.columns + j];
}

std::size_t
oper12_view::rows() const noexcept
{
  return accel::rows(*value_);
}

std::size_t
oper12_view::cols() const noexcept
{
  return accel::cols(*value_);
}

std::size_t
oper12_view::size() const noexcept
{
  return rows() * cols();
}

}  // namespace accel
}  // namespace oa
