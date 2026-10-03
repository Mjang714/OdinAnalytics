/**
 * @file features.h
 * @author Derek Huang
 * @brief C/C++ header for feature checking
 * @copyright MIT License
 */

#ifndef OA_FEATURES_H_
#define OA_FEATURES_H_

#include "oa/common.h"  // for OA_CPLUSPLUS

////////////////////////////////////////////////////////////////////////////////
// C++ language features                                                      //
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// C++ library features                                                       //
////////////////////////////////////////////////////////////////////////////////

// we support much less "auto-detection" when it comes to libraries because
// different versions of the same library may be incompatible. or, for example,
// a version of the library installed in a system location will automatically
// cause __has_include() based detection to evaluate to 1, even if one doesn't
// want to use said header in their own code, which can be problematic when
// there are incompatibilities between library headers.
//
// the convention we use still follows the OA_HAS_<feature> format, but for
// build system convenience, OA_HAS_<feature> is defined to 1 if compiling with
// OA_USE_<feature> defined. OA_USE_<feature> overrides compiling with the
// feature disabled, e.g. defining OA_HAS_<feature>=0 during compilation.

// check if OpenBLAS is available
#ifdef OA_USE_OPENBLAS
#undef OA_HAS_OPENBLAS
#define OA_HAS_OPENBLAS 1
#endif  // OA_USE_OPENBLAS
#ifndef OA_HAS_OPENBLAS
#define OA_HAS_OPENBLAS 0
#endif // OA_HAS_OPENBLAS

// check if Eigen3 is available
#ifdef OA_USE_EIGEN3
#undef OA_HAS_EIGEN3
#define OA_HAS_EIGEN3 1
#endif  // OA_USE_EIGEN3
#ifndef OA_HAS_EIGEN3
#define OA_HAS_EIGEN3 0
#endif  // OA_HAS_EIGEN3

// check if Armadillo is available
#ifdef OA_USE_ARMADILLO
#undef OA_HAS_ARMADILLO
#define OA_HAS_ARMADILLO 1
#endif  // OA_UAS_ARMADILLO
#ifndef OA_HAS_ARMADILLO
#define OA_HAS_ARMADILLO 0
#endif  // OA_HAS_ARMADILLO

#endif  // OA_FEATURES_H_
