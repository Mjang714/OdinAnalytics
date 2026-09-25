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

// check if OpenBLAS is available
#ifdef __has_include
#if __has_include(<openblas_config.h>)
#define OA_HAS_OPENBLAS 1
#endif  // __has_include(<openblas_config.h>)
#endif  // __has_include

#ifndef OA_HAS_OPENBLAS
#define OA_HAS_OPENBLAS 0
#endif // OA_HAS_OPENBLAS

// check if Eigen3 is available
#ifdef __has_include
#if __has_include(<Eigen/Core>)
#define OA_HAS_EIGEN3 1
#endif  // __has_include(<Eigen/Core>)
#endif  // __has_include

#ifndef OA_HAS_EIGEN3
#define OA_HAS_EIGEN3 0
#endif  // OA_HAS_EIGEN3

// check if Armadillo is available
#ifdef __has_include
#if __has_include(<armadillo>)
#define OA_HAS_ARMADILLO 1
#endif  // __has_include(<armadillo>)
#endif  // __has_include

#ifndef OA_HAS_ARMADILLO
#define OA_HAS_ARMADILLO 0
#endif  // OA_HAS_ARMADILLO

#endif  // OA_FEATURES_H_
