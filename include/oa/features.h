/**
 * @file features.h
 * @author Derek Huang
 * @brief C/C++ header for feature checking
 * @copyright MIT License
 */

#ifndef OA_FEATURES_H_
#define OA_FEATURES_H_

#include "oa/common.h"  // for OA_CPLUSPLUS

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

#endif  // OA_FEATURES_H_
