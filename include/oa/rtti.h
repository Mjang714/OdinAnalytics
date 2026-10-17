/**
 * @file rtti.h
 * @author Derek Huang
 * @brief C++ header for RTTI helpers
 * @copyright MIT License
 */

#ifndef OA_RTTI_H_
#define OA_RTTI_H_

#include <string_view>
#include <typeinfo>

// TODO: refactor existing code to only include ctti.h as necessary. rtti.h
// should only deal with run-time type information helpers
#include "oa/ctti.h"
#include "oa/features.h"

#if OA_HAS_ITANIUM_ABI
#include <cxxabi.h>     // abi::__cxa_demangle()

#include <cstdlib>      // std::free()
#include <memory>
#include <stdexcept>
#else
#include <string>
#endif  // !OA_HAS_ITANIUM_ABI

// 1 if run-time type information functionality is enabled, 0 otherwise. one
// can compile without RTTI with MSVC by using /GR-, -fno-rtti for GCC/Clang.
// MSVC defined _CPPRTTI, GCC defines __GXX_RTTI when RTTI is enabled
#if defined(_CPPRTTI) || defined(__GXX_RTTI)
#define OA_RTTI_ENABLED 1
#endif  // !defined(_CPPRTTI) && !defined(__GXX_RTTI)
// Clang uses __has_feature(cxx_rtti) to check
#if !defined(OA_RTTI_ENABLED) && defined(__has_feature)
#define OA_RTTI_ENABLED __has_feature(cxx_rtti)
#endif   // defined(OA_RTTI_ENABLED) || !defined(__has_feature)
// otherwise, compiled without RTTI
#ifndef OA_RTTI_ENABLED
#define OA_RTTI_ENABLED 0
#endif  // OA_RTTI_ENABLED

namespace oa {

/**
 * Return the demangled type name from the mangled type name.
 *
 * The input value should be be the null-terminated string returned by
 * `std::type_info::name()`. For compilers that follow the Itanium C++ ABI,
 * e.g. GCC, Clang, this named is mangled, and this function uses the
 * `abi::__cxa_demangle()` function to demangle the name. For other compilers,
 * e.g. MSVC, the input value is simply returned as-is.
 *
 * @param name Possibly-mangled null-terminated type name
 */
inline const char* demangle(const char* name)
{
#if OA_HAS_ITANIUM_ABI
  // local deleter type that uses std::free()
  struct free_deleter {
    void operator()(void* mem) const noexcept
    {
      std::free(mem);
    }
  };
  // unique_ptr type using free_deleter + thread-local buffer to manage
  using unique_buffer = std::unique_ptr<char[], free_deleter>;
  thread_local unique_buffer buf;
  // demangle and handle errors
  int err;
  buf = unique_buffer{abi::__cxa_demangle(name, nullptr, nullptr, &err)};
  switch (err) {
  case 0:
    return buf.get();
  case -1:
    throw std::runtime_error{"__cxa_demangle could not allocate memory"};
  case -2:
    throw std::runtime_error{"invalid mangled name"};
  case -3:
    throw std::runtime_error{"__cxa_demangle given invalid argument"};
  default:
    throw std::logic_error{"unexpected __cxa_demangle error return"};
  }
#else
  // copy to thread-local buffer + return pointer
  thread_local std::string buf;
  buf = name;
  return buf.c_str();
#endif  // !OA_HAS_ITANIUM_ABI
}

#if OA_RTTI_ENABLED
/**
 * Return the `std::type_info` demangled type name as a `std::string_view`.
 *
 * @param info Type info object, e.g. from using `typeid`
 */
inline auto type_name(const std::type_info& info)
{
  return std::string_view{demangle(info.name())};
}
#endif  // OA_RTTI_ENABLED

}  // namespace oa

#endif  // OA_RTTI_H_
