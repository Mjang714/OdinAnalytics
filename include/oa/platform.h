/**
 * @file platform.h
 * @author Derek Huang
 * @brief C/C++ header for platform feature checking
 * @copyright MIT License
 */

#ifndef OA_PLATFORM_H_
#define OA_PLATFORM_H_

// accurate C++ version integer, no need to use /Zc:__cplusplus with MSVC
#if defined(_MSC_VER) && defined(_MSVC_LANG)
#define OA_CPLUSPLUS _MSVC_LANG
#elif defined(__cplusplus)
#define OA_CPLUSPLUS __cplusplus
#endif  // (!defined(_MSC_VER) || !defined(_MSVC_LANG)) && !defined(__cplusplus)

// check if we are compiling under C++20 or above. always define when compiling
// under C++ so we could use this in standard C++ expressions
#ifdef OA_CPLUSPLUS
#if OA_CPLUSPLUS >= 202002L
#define OA_CPLUSPLUS_20 1
#else
#define OA_CPLUSPLUS_20 0
#endif  // OA_CPLUSPLUS < 202002L
#endif  // OA_CPLUSPLUS

// check if we have the C++20 <format> header. __has_include is standard since
// C++17, available as compiler extension for C or earlier C++ standards
#if defined(OA_CPLUSPLUS) && defined(__has_include)
#if OA_CPLUSPLUS_20 && __has_include(<format>)
#define OA_HAS_CPP20_FORMAT 1
#else
#define OA_HAS_CPP20_FORMAT 0
#endif  // !OA_CPLUSPLUS || !__has_include(<format>)
#endif  // !defined(OA_CPLUSPLUS) || !defined(__has_include)

#endif  // OA_PLATFORM_H_
