/**
 * @file accel/oper12.cpp
 * @author Derek Huang
 * @brief C++ source for the Excel 12 fundamental data type (operand)
 * @copyright MIT License
 */

#include "oa/accel/oper12.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <concepts>
#include <cstddef>
#include <cstring>
#include <iostream>  // for std::wcout
#include <limits>
#include <optional>
#include <memory>
#include <string>
#include <utility>

#include "oa/accel/mref12.h"

namespace oa {
namespace accel {

namespace {

/**
 * Helper function to create a string `XLOPER12` from a character buffer.
 *
 * This copies the data and appropriately prepends the 2-byte length, which
 * must be 65535 or less, and appends the wide null terminator. The data must
 * be freed using `delete[]` or will be leaked.
 *
 * @tparam T Character type
 *
 * @param out `XLOPER12` to populate
 * @param buf Character buffer
 * @param length Number of characters to copy
 */
template <typename T>
requires (std::same_as<T, char> || std::same_as<T, WCHAR>)
void to(xloper12* out, const T* buf, std::size_t length)
{
  // max length
  constexpr auto max_len = (std::numeric_limits<WCHAR>::max)();
  // throw an exception if the string is too long as 65535 is the max length
  if (length >= max_len)
    throw std::runtime_error{
      "string size " + std::to_string(length) + " exceeds max " +
      std::to_string(max_len)
    };
  // allocate new wide character data. we need 1 starting wide char for the
  // data length (excluding null) + 1 and 1 trailing null terminator
  auto data = std::make_unique<WCHAR[]>(length + 2);
  // write size
  data[0] = static_cast<WCHAR>(length);
  // widen characters as necessary (copy if WCHAR already)
  if constexpr (std::same_as<T, WCHAR>)
    std::memcpy(&data[1], buf, sizeof(WCHAR) * length);
  else {
    for (std::size_t i = 0u; i < length; i++)
      data[i + 1] = std::wcout.widen(buf[i]);
  }
  // null-terminate
  data[length + 1] = L'\0';
  // update XLOPER12 values
  out->val.str = data.release();
  out->xltype = xltypeStr;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// ctors + assignment + dtors                                                 //
////////////////////////////////////////////////////////////////////////////////

oper12::oper12(oper12&& other) noexcept
{
  from(std::move(other));
}

oper12&
oper12::operator=(oper12&& other) noexcept
{
  destroy();
  from(std::move(other));
  return *this;
}

oper12::~oper12()
{
  destroy();
}

////////////////////////////////////////////////////////////////////////////////
// value constructors                                                         //
////////////////////////////////////////////////////////////////////////////////

oper12::oper12(xlerr err) : value_{new xloper12{}}
{
  value_->val.err = static_cast<int>(err);
  value_->xltype = xltypeErr;
}

oper12::oper12(bool val) : value_{new xloper12{}}
{
  value_->val.xbool = val;
  value_->xltype = xltypeBool;
}

oper12::oper12(int val) : value_{new xloper12{}}
{
  value_->val.w = val;
  value_->xltype = xltypeInt;
}

oper12::oper12(double val) : value_{new xloper12{}}
{
  value_->val.num = val;
  value_->xltype = xltypeNum;
}

oper12::oper12(std::string_view str)
{
  // use unique_ptr to be exception-safe if to() throws
  auto val = std::make_unique<xloper12>();
  to(val.get(), str.data(), str.size());
  // update value_ + owning_
  value_ = val.release();
  owning_ = true;
}

oper12::oper12(std::wstring_view str)
{
  // use unique_ptr to be exception-safe if to() throws
  auto val = std::make_unique<xloper12>();
  to(val.get(), str.data(), str.size());
  // update value_ + owning_
  value_ = val.release();
  owning_ = true;
}

oper12::oper12(xlref12 ref) : value_{new xloper12{}}
{
  value_->val.sref = {1u, ref};
  value_->xltype = xltypeSRef;
}

oper12::oper12(const mref12& mref) : oper12{0u, mref} {}

oper12::oper12(std::uintptr_t id, const mref12& mref)
{
  // use unique_ptr to be exception-safe if mref() throws
  auto val = std::make_unique<xloper12>();
  auto mref_new = mref;
  // update members
  val->val.mref.lpmref = mref_new.release();
  val->val.mref.idSheet = id;
  val->xltype = xltypeRef;
  // update value_ + owning_
  value_ = val.release();
  owning_ = true;
}

oper12::oper12(mref12&& mref) : oper12{0u, std::move(mref)} {}

oper12::oper12(std::uintptr_t id, mref12&& mref) : value_{new xloper12{}}
{
  value_->val.mref.lpmref = mref.release();  // noexcept
  value_->val.mref.idSheet = id;
  value_->xltype = xltypeRef;
  owning_ = true;
}

////////////////////////////////////////////////////////////////////////////////
// nil + missing                                                              //
////////////////////////////////////////////////////////////////////////////////

oper12
oper12::nil()
{
  oper12 res;
  res.value_ = new xloper12;
  res.value_->xltype = xltypeNil;
  return res;
}

oper12
oper12::missing()
{
  oper12 res;
  res.value_ = new xloper12;
  res.value_->xltype = xltypeMissing;
  return res;
}

////////////////////////////////////////////////////////////////////////////////
// public member functions                                                    //
////////////////////////////////////////////////////////////////////////////////

xloper12*
oper12::value() noexcept
{
  return value_;
}

const xloper12*
oper12::value() const noexcept
{
  return value_;
}

bool
oper12::owning() const noexcept
{
  return owning_;
}

xloper12*
oper12::release() noexcept
{
  // copy value_ and clear
  auto res = value_;
  value_ = nullptr;
  // if owning, set xlbitDLLFree
  if (owning_)
    res->xltype |= xlbitDLLFree;
  // otherwise, assume Excel owns the memory, and set xlbitXLFree if XLOPER12
  // type is one of the Excel types that require extra memory
  else if (needs_extra_memory(type()))
    res->xltype |= xlbitXLFree;
  // done
  return res;
}

xltype
oper12::type() const noexcept
{
  // cast to satisfy initialization rules
  return xltype{static_cast<int>(value_->xltype)};
}

std::optional<xlerr>
oper12::error() const noexcept
{
  if (type() == xltype::err)
    return xlerr{value_->val.err};
  else
    return {};
}

////////////////////////////////////////////////////////////////////////////////
// private member functions                                                   //
////////////////////////////////////////////////////////////////////////////////

void
oper12::from(oper12&& other) noexcept
{
  value_ = other.value_;
  owning_ = other.owning_;
  other.value_ = nullptr;
  other.owning_ = false;
}

void
oper12::destroy() noexcept
{
  // do nothing if no data
  if (!value_)
    return;
  // if owning, we free XLOPER12 memory ourselves
  if (owning_) {
    // TODO: this can be broken out into a xlAutoFree12() implementation
    switch (value_->xltype) {
    // string
    case xltypeStr:
      delete[] value_->val.str;
      break;
    // multi-cell reference
    case xltypeRef:
      xlmref12_free(value_->val.mref.lpmref);
      break;
    // array
    // note: we don't support constructing xltypeMulti currently
    case xltypeMulti: {
      // rows + columns
      auto n_rows = value_->val.array.rows;
      auto n_cols = value_->val.array.columns;
      // lambda for xloper12 (i, j) array reference
      auto array = [this, n_rows, n_cols](auto i, auto j) noexcept -> auto&
      {
        return value_->val.array.lparray[i * n_rows + j * n_cols];
      };
      // any xltypeStr objects are deleted like in the xltypeStr case, but we
      // specifically mask off 0xFFF to correctly ignore the xlbit* values
      for (decltype(n_rows) i = 0; i < n_rows; i++)
        for (decltype(n_cols) j = 0; j < n_cols; j++)
          if ((array(i, j).xltype & 0xFFF) == xltypeStr)
            delete[] array(i, j).val.str;
      break;
    }
    }
  }
  // otherwise, call Excel12(xlFree, ...) as appropriate. see
  // https://learn.microsoft.com/en-us/office/client-developer/excel/xlfree
  else if (needs_extra_memory(type()))
    Excel12(xlFree, nullptr, 1, value_);
  // now we can delete the XLOPER12 itself
  delete value_;
}

}  // namespace accel
}  // namespace oa
