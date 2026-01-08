/**
 * @file mref12.cpp
 * @author Derek Huang
 * @brief C++ source for the Excel 12 multi-reference type
 * @copyright MIT License
 */

#include "oa/accel/mref12.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include "oa/accel/xl_ops.h"  // for xlref12 operator==

namespace oa {
namespace accel {

////////////////////////////////////////////////////////////////////////////////
// raw functions                                                              //
////////////////////////////////////////////////////////////////////////////////

xlmref12* xlmref12_malloc(WORD count)
{
  // if zero, no allocation
  if (!count)
    return nullptr;
  // otherwise, allocate sizeof(xlmref12) + sizeof(xlref12) * (count - 1) bytes
  auto buf = new unsigned char[sizeof(xlmref12) + sizeof(xlref12) * (count - 1u)];
  // assign using placement new + update count
  auto res = new(buf) xlmref12;
  res->count = count;
  return res;
}

xlmref12* xlmref12_copy(xlmref12* mref)
{
  // if nullptr, return nullptr
  if (!mref)
    return nullptr;
  // otherwise allocate
  auto res = xlmref12_malloc(mref->count);
  // copy the struct element + variable length data
  res->reftbl[0] = mref->reftbl[0];
  std::memcpy(
    reinterpret_cast<unsigned char*>(res) + sizeof(xlmref12),
    reinterpret_cast<const unsigned char*>(mref) + sizeof(xlmref12),
    sizeof(xlref12) * (mref->count - 1u)
  );
  return res;
}

void xlmref12_free(xlmref12* mref) noexcept
{
  // no-op if nullptr
  if (!mref)
    return;
  // otherwise call dtor + free
  // note: although POD we call dtor as a technicality
  mref->~xlmref12();
  delete[] mref;
}

////////////////////////////////////////////////////////////////////////////////
// ctors + dtor + assignment operators                                        //
////////////////////////////////////////////////////////////////////////////////

mref12::mref12() : mref12{0u} {}

mref12::mref12(const mref12& other)
{
  from(other);
}

mref12&
mref12::operator=(const mref12& other)
{
  destroy();
  from(other);
  return *this;
}

mref12::mref12(mref12&& other) noexcept
{
  from(std::move(other));
}

mref12&
mref12::operator=(mref12&& other) noexcept
{
  destroy();
  from(std::move(other));
  return *this;
}

mref12::~mref12()
{
  destroy();
}

///////////////////////////////////////////////////////////////////////////////
// value constructors                                                        //
///////////////////////////////////////////////////////////////////////////////

mref12::mref12(std::size_t count)
{
  // max count
  constexpr auto max_count = (std::numeric_limits<WORD>::max)();
  // if count too large, error
  if (count > max_count)
    throw std::runtime_error{
      "count " + std::to_string(count) + " exceeded maximum value " +
      std::to_string(max_count) + " supported by xlmref12"
    };
  // otherwise, allocate new xlmref12 variable-length data
  value_ = xlmref12_malloc(static_cast<WORD>(count));
}

mref12::mref12(std::initializer_list<xlref12> refs) : mref12{refs.size()}
{
  // if zero, skip
  if (!refs.size())
    return;
  // copy first + memcpy() the rest
  value_->reftbl[0] = *refs.begin();
  std::memcpy(
    reinterpret_cast<unsigned char*>(value_) + sizeof(xlmref12),
    refs.begin() + 1u,
    sizeof(xlref12) * (refs.size() - 1u)
  );
}

////////////////////////////////////////////////////////////////////////////////
// public member functions                                                    //
////////////////////////////////////////////////////////////////////////////////

bool
mref12::operator==(const mref12& other) const noexcept
{
  // not equal if size differs
  if (size() != other.size())
    return false;
  // ok, elementwise comparison
  for (std::size_t i = 0u; i < size(); i++)
    // note: operator!= synthesized from operator== in C++20
    if ((*this)[i] != other[i])
      return false;
  return true;
}

xlmref12*
mref12::release() noexcept
{
  auto res = value_;
  value_ = nullptr;
  return res;
}

xlref12*
mref12::data() noexcept
{
  return value_->reftbl;
}

const xlref12*
mref12::data() const noexcept
{
  return value_->reftbl;
}

bool
mref12::empty() const noexcept
{
  return !value_;
}

std::size_t
mref12::size() const noexcept
{
  return (value_) ? value_->count : 0u;
}

xlref12&
mref12::operator[](std::size_t i) noexcept
{
  return data()[i];
}

const xlref12&
mref12::operator[](std::size_t i) const noexcept
{
  return data()[i];
}

xlref12*
mref12::begin() noexcept
{
  return data();
}

const xlref12*
mref12::begin() const noexcept
{
  return data();
}

xlref12*
mref12::end() noexcept
{
  return data() + size();
}

const xlref12*
mref12::end() const noexcept
{
  return data() + size();
}

////////////////////////////////////////////////////////////////////////////////
// private member functions                                                   //
////////////////////////////////////////////////////////////////////////////////

void
mref12::from(const mref12& other)
{
  value_ = xlmref12_copy(other.value_);
}

void
mref12::from(mref12&& other) noexcept
{
  value_ = other.value_;
  other.value_ = nullptr;
}

void
mref12::destroy() noexcept
{
  xlmref12_free(value_);
}

}  // namespace accel
}  // namespace oa
