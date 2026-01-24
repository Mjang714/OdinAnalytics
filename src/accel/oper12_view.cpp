/**
 * @file oper12_view.cpp
 * @author Derek Huang
 * @brief C++ source for view types over the Excel 12 fundamental data type
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
#include <ostream>

#include "oa/accel/enums.h"
#include "oa/accel/xl_ops.h"

namespace oa {
namespace accel {

////////////////////////////////////////////////////////////////////////////////
// oper12_view                                                                //
////////////////////////////////////////////////////////////////////////////////

oper12_view::oper12_view() noexcept : oper12_view(nullptr) {}

oper12_view::oper12_view(const xloper12* value) noexcept : value_{value} {}

bool
oper12_view::operator!() const noexcept
{
  return !value_;
}

const xloper12*
oper12_view::operator->() const noexcept
{
  return value_;
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

oper12_row_view
oper12_view::operator[](std::size_t i) const noexcept
{
  // use unsigned to avoid C2398 on init
  unsigned n_cols = value_->val.array.columns;
  return {&value_->val.array.lparray[i * n_cols], n_cols};
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

////////////////////////////////////////////////////////////////////////////////
// oper12_row_view                                                            //
////////////////////////////////////////////////////////////////////////////////

oper12_row_view::oper12_row_view(const xloper12* data, std::size_t size) noexcept
  : data_{data}, size_{size}
{}

oper12_view
oper12_row_view::operator[](std::size_t i) const noexcept
{
  return data_ + i;
}

const xloper12*
oper12_row_view::data() const noexcept
{
  return data_;
}

std::size_t
oper12_row_view::size() const noexcept
{
  return size_;
}

////////////////////////////////////////////////////////////////////////////////
// operator<<                                                                 //
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const oper12_view& op)
{
  out << "[view] ";
  // empty
  if (!op)
    return out << "(empty)";
  // otherwise, format value
  return out << *op.operator->();
}

}  // namespace accel
}  // namespace oa
