/**
 * @file rtti.h
 * @author Derek Huang
 * @brief C++ header for RTTI helpers
 * @copyright MIT License
 */

#ifndef OA_RTTI_H_
#define OA_RTTI_H_

#include <typeinfo>

// FIXME: don't use Boost for this task (forced usage requirement). we want to
// avoid a find_dependency() call in the CMake config file; just use
// abi::__cxa_demangle directly for this task for GCC/Clang
#include <boost/core/demangle.hpp>

// TODO: refactor existing code to only include ctti.h as necessary. rtti.h
// should only deal with run-time type information helpers
#include "oa/ctti.h"

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

#ifdef OA_RTTI_ENABLED
namespace oa {

/**
 * Return the demangled name of a `std::type_info` object.
 *
 * @param info Type info object, e.g. from using `typeid`
 * @returns `std::string` containing the demangled type name
 */
inline auto demangled_name(const std::type_info& info)
{
  return boost::core::demangle(info.name());
}

}  // namespace oa

/**
 * Return the demangled name of a type or of an expression's type.
 *
 * @param expr Type or expression
 * @returns `std::string` containing the demangled type name
 */
#define OA_DEMANGLED_NAME(expr) oa::demangled_name(typeid(expr))
#endif  // OA_RTTI_ENABLED

#endif  // OA_RTTI_H_
