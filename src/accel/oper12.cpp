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
#include <ostream>
#include <memory>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "oa/accel/enums.h"
#include "oa/accel/mref12.h"
#include "oa/accel/xl_ops.h"  // for operator<<
#include "oa/string.h"        // for oa::hex

namespace oa {
namespace accel {

namespace {

/**
 * Helper function to create a string `XLOPER12` from a character buffer.
 *
 * This copies the string data and appropriately prepends the 2-byte length,
 * which must be 65535 or less. Note that the string is *not* null-terminated.
 * The data must later be freed using `delete[]` or will be leaked.
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
  // data length (excluding null) + 1 (no null terminator)
  auto data = std::make_unique<WCHAR[]>(length + 1);
  // write size
  data[0] = static_cast<WCHAR>(length);
  // widen characters as necessary (copy if WCHAR already)
  if constexpr (std::same_as<T, WCHAR>)
    std::memcpy(&data[1], buf, sizeof(WCHAR) * length);
  else {
    for (std::size_t i = 0u; i < length; i++)
      data[i + 1] = std::wcout.widen(buf[i]);
  }
  // update XLOPER12 values
  out->val.str = data.release();
  out->xltype = xltypeStr;
}

/**
 * Helper function to create an array `XLOPER12` from a floating-point span.
 *
 * This copies the span data and produces a single-column `xltypeMulti` array.
 * The allocated `array.lparray` member must be freed with `delete[]` to
 * prevent memory from leaking if an exception is thrown.
 *
 * @tparam T Floating-point type
 *
 * @param out `XLOPER12` to populate
 * @param vals Input span
 */
template <std::floating_point T>
void to(xloper12* out, std::span<const T> vals)
{
  // max length
  constexpr auto max_rows = (std::numeric_limits<RW>::max)();
  // throw if span size would cause an overflow
  if (vals.size() > max_rows)
    throw std::runtime_error{
      "span size " + std::to_string(vals.size()) + " exceeds max " +
      std::to_string(max_rows)
    };
  // since no extra memory is needed for doubles we directly allocate buffer
  auto buf = std::make_unique<xloper12[]>(vals.size());
  // populate each xloper12 in array
  for (auto i = 0u; i < vals.size(); i++) {
    buf[i].val.num = vals[i];
    buf[i].xltype = xltypeNum;
  }
  // update XLOPER12 members
  out->val.array.lparray = buf.release();
  out->val.array.rows = static_cast<RW>(vals.size());
  out->val.array.columns = 1;
  out->xltype = xltypeMulti;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// raw functions                                                              //
////////////////////////////////////////////////////////////////////////////////

xloper12* xloper12_copy(const xloper12* op)
{
  // no-op if nullptr
  if (!op)
    return nullptr;
  // allocate (unique_ptr for exception safety) copy + mask deallocation bits
  auto res = std::make_unique<xloper12>(*op);
  res->xltype &= ~xlbitDLLFree;
  res->xltype &= ~xlbitXLFree;
  // handle types with auxiliary memory requirements
  switch (res->xltype) {
  // string
  case xltypeStr: {
    // Excel strings have size encoded in first character
    auto len = op->val.str[0] + 1u;
    auto buf = std::make_unique<XCHAR[]>(len);
    std::memcpy(buf.get(), op->val.str, sizeof(XCHAR) * len);
    // update res + release
    res->val.str = buf.release();
    break;
  }
  // multi-cell reference
  case xltypeRef:
    res->val.mref.lpmref = xlmref12_copy(op->val.mref.lpmref);
    break;
  // array
  case xltypeMulti: {
    // total size
    unsigned len = op->val.array.rows * op->val.array.columns;
    // allocate with scoping for exception safety + copy XLOPER12 array
    auto buf = std::make_unique<xloper12[]>(len);
    std::memcpy(buf.get(), op->val.array.lparray, sizeof(xloper12) * len);
    // indices + buffers for any string data that needs copying
    std::vector<std::pair<unsigned, std::unique_ptr<XCHAR[]>>> strs;
    // copy memory for any allocated strings
    for (auto i = 0u; i < len; i++) {
      // reference to ith XLOPER12
      const auto& op_i = op->val.array.lparray[i];
      // handle strings
      if (op_i.xltype == xltypeStr) {
        // again, Excel strings have size in first char
        auto slen = op_i.val.str[0] + 1u;
        strs.push_back({i, std::make_unique<XCHAR[]>(slen)});
        std::memcpy(strs.back().second.get(), op_i.val.str, sizeof(XCHAR) * slen);
      }
    }
    // if no allocation exceptions thrown, release memory to new array
    // note: type checking already done in previous loop
    for (auto i = 0u; i < strs.size(); i++)
      buf[strs[i].first].val.str = strs[i].second.release();
    // update res + release
    res->val.array.lparray = buf.release();
    break;
  }
  // other types
  default:
    break;
  }
  // done, so release res itself
  return res.release();
}

void xloper12_free(xloper12* op) noexcept
{
  // no-op if nullptr
  if (!op)
    return;
  // handle types
  switch (op->xltype & ~xlbitDLLFree) {
  // string
  case xltypeStr:
    delete[] op->val.str;
    break;
  // multi-cell reference
  case xltypeRef:
    xlmref12_free(op->val.mref.lpmref);
    break;
  // array
  case xltypeMulti: {
    // total size
    unsigned len = op->val.array.rows * op->val.array.columns;
    // any xltypeStr objects are deleted like in the xltypeStr case
    for (auto i = 0u; i < len; i++) {
      // reference to ith XLOPER12
      const auto& op_i = op->val.array.lparray[i];
      // handle strings
      if (op_i.xltype == xltypeStr)
        delete[] op_i.val.str;
    }
    // delete array itself
    delete[] op->val.array.lparray;
    break;
  }
  // other types
  default:
    break;
  }
  // done, so delete op itself
  delete op;
}

////////////////////////////////////////////////////////////////////////////////
// ctors + assignment + dtors                                                 //
////////////////////////////////////////////////////////////////////////////////

oper12::oper12() : value_{new xloper12}
{
  value_->xltype = xltypeNil;
}

oper12::oper12(const oper12& other)
{
  from(other);
}

oper12&
oper12::operator=(const oper12& other)
{
  destroy();
  from(other);
  return *this;
}

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

oper12::oper12(const char* str) : oper12{std::string_view{str}} {}

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

oper12::oper12(std::int32_t row, std::int32_t col)
  : oper12{{row, row, col, col}}
{}

oper12::oper12(xlref12 ref) : value_{new xloper12{}}
{
  value_->val.sref = {1u, ref};
  value_->xltype = xltypeSRef;
}

oper12::oper12(const mref12& mref)
{
  // use unique_ptr to be exception-safe if mref() throws
  auto val = std::make_unique<xloper12>();
  auto mref_new = mref;
  // update members
  val->val.mref.lpmref = mref_new.release();
  val->val.mref.idSheet = mref_new.sheet();
  val->xltype = xltypeRef;
  // update value_ + owning_
  value_ = val.release();
  owning_ = true;
}

oper12::oper12(mref12&& mref) : value_{new xloper12{}}
{
  value_->val.mref.lpmref = mref.release();  // noexcept
  value_->val.mref.idSheet = mref.sheet();   // noexcept
  value_->xltype = xltypeRef;
  owning_ = true;
}

oper12::oper12(std::span<const float> vals)
{
  // use unique_ptr to be exception-safe if to() throws
  auto val = std::make_unique<xloper12>();
  to(val.get(), vals);
  // update value_ + owning
  value_ = val.release();
  owning_ = true;
}

oper12::oper12(std::span<const double> vals)
{
  // use unique_ptr to be exception-safe if to() throws
  auto val = std::make_unique<xloper12>();
  to(val.get(), vals);
  // update value_ + owning
  value_ = val.release();
  owning_ = true;
}

oper12::oper12(const char* data, std::size_t size)
  : oper12{reinterpret_cast<const unsigned char*>(data), size}
{}

oper12::oper12(const unsigned char* data, std::size_t size)
{
  // max data length
  constexpr auto max_len = (std::numeric_limits<long>::max)();
  // size must not exceed long max
  if (size > max_len)
    throw std::runtime_error{
      "bigdata length " + std::to_string(size) + " exceeded maximum " +
      std::to_string(max_len)
    };
  // initialize
  value_ = new xloper12{};
  value_->val.bigdata.h.lpbData = const_cast<BYTE*>(data);
  value_->val.bigdata.cbData = static_cast<long>(size);
  value_->xltype = xltypeBigData;
}

////////////////////////////////////////////////////////////////////////////////
// nil + missing                                                              //
////////////////////////////////////////////////////////////////////////////////

oper12
oper12::nil()
{
  return {};
}

oper12
oper12::missing()
{
  oper12 res;
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

oper12::operator bool() const noexcept
{
  return !!value_;
}

bool
oper12::owning() const noexcept
{
  return owning_;
}

xloper12*
oper12::release() noexcept
{
  // if owning, set xlbitDLLFree
  if (owning_)
    value_->xltype |= xlbitDLLFree;
  // otherwise, assume Excel owns the memory, and set xlbitXLFree if XLOPER12
  // type is one of the Excel types that require extra memory
  else if (needs_aux_memory(type()))
    value_->xltype |= xlbitXLFree;
  // copy value_, clear, and return
  auto res = value_;
  value_ = nullptr;
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
oper12::from(const oper12& other)
{
  value_ = xloper12_copy(other.value_);
  switch (value_->xltype) {
  case xltypeStr:
  case xltypeRef:
  case xltypeMulti:
    owning_ = true;
    break;
  default:
    owning_ = false;
    break;
  }
}

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
    xloper12_free(value_);
    return;
  }
  // otherwise, call Excel12(xlFree, ...) as appropriate. see
  // https://learn.microsoft.com/en-us/office/client-developer/excel/xlfree
  if (needs_aux_memory(type()))
    Excel12(xlFree, nullptr, 1, value_);
  // now we can delete the XLOPER12 itself
  delete value_;
}

////////////////////////////////////////////////////////////////////////////////
// operator<<                                                                 //
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const oper12& op)
{
  // print ownership
  out << "[owning=" <<
    [own = op.owning()] { return (own) ? "true" : "false"; }() << "] ";
  // empty
  if (!op.value())
    return out << "(empty)";
  // otherwise, format value
  return out << *op.value();
}

////////////////////////////////////////////////////////////////////////////////
// to_string()                                                                //
////////////////////////////////////////////////////////////////////////////////

std::string to_string(const oper12& op)
{
  std::stringstream ss;
  ss << op;
  return std::move(ss).str();  // ref-qualified overload since C++20
}

}  // namespace accel
}  // namespace oa
