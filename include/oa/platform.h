/**
 * @file platform.h
 * @author Derek Huang
 * @brief C/C++ header for platform feature checking
 * @copyright MIT License
 */

#ifndef OA_PLATFORM_H_
#define OA_PLATFORM_H_

#include "oa/common.h"

// TODO: rename header to features.h as not all features are platform features

// check if we are compiling under C++20 or above. always define when compiling
// under C++ so we could use this in standard C++ expressions
#ifdef OA_CPLUSPLUS
#if OA_CPLUSPLUS >= 202002L
#define OA_HAS_CXX20 1
#endif  // OA_CPLUSPLUS >= 202002L
#endif  // OA_CPLUSPLUS

#ifndef OA_HAS_CXX20
#define OA_HAS_CXX20 0
#endif  // OA_HAS_CXX20

// check if we have the C++20 <format> header. __has_include is standard since
// C++17, available as compiler extension for C or earlier C++ standards
#if defined(OA_CPLUSPLUS) && defined(__has_include)
#if OA_HAS_CXX20 && __has_include(<format>)
#define OA_HAS_CXX20_FORMAT 1
#endif  // OA_CPLUSPLUS && __has_include(<format>)
#endif  // !defined(OA_CPLUSPLUS) || !defined(__has_include)

#ifndef OA_HAS_CXX20_FORMAT
#define OA_HAS_CXX20_FORMAT 0
#endif  // OA_HAS_CXX20_FORMAT

// TODO: deprecate OA_HAS_CPP20_FORMAT and use OA_HAS_CXX20_FORMAT instead
#define OA_HAS_CPP20_FORMAT OA_HAS_CXX20_FORMAT

#endif  // OA_PLATFORM_H_
