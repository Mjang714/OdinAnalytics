/**
 * @file ctti.h
 * @author Derek Huang
 * @brief C++ header for compile-time type information
 * @copyright MIT License
 */

#ifndef OA_CTTI_H_
#define OA_CTTI_H_

#include <cstddef>

#include "oa/common.h"
#include "oa/fixed_string.h"

// unqualified name of the current function
#define OA_FUNCTION_NAME __func__

// qualified signature of current function, i.e. including class name, params.
// __PRETTY_FUNCTION__ is used for GCC-like compilers, __FUNCSIG__ for MSVC
#if defined(_MSC_VER)
#define OA_PRETTY_FUNCTION_NAME __FUNCSIG__
#elif defined(__GNUC__)
#define OA_PRETTY_FUNCTION_NAME __PRETTY_FUNCTION__
// for other compilers, just fall back to OA_FUNCTION_NAME
#else
#define OA_PRETTY_FUNCTION_NAME OA_FUNCTION_NAME
#endif  // !defined(_MSC_VER) && !defined(__GNUC__)

/**
 * Return a `fixed_string<N>` representing the current call site.
 *
 * This provides only the file name and line number for brevity of reporting.
 * For example, one possible value could be `"xl_array.cpp:148"`.
 */
#define OA_SOURCE_LOCATION() \
  oa::fixed_string{__FILE__} \
    /* ensure we have only file name without directory separators. */ \
    /* note that we need to check for both '/' and '\\' */ \
    .substr<oa::fixed_string{__FILE__}.rfind('/', '\\') + 1u>() + \
    /* source file line */ \
    ":" OA_STRINGIFY(__LINE__)

#endif  // OA_CTTI_H_
