/**
 * @file dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for DLL export macros
 * @copyright MIT License
 */

#ifndef OA_DLLEXPORT_H_
#define OA_DLLEXPORT_H_

/**
 * Guide to Visibility Macros
 *
 * Odin libraries can be built as either static or dynamic libraries. For any
 * shared library `lib<PROJECT>.so` for *nix or `<PROJECT>.dll` on Windows, we
 * describe the relevant macros and explain when to use them.
 *
 * `OA_DLL`
 *  Define when building or using *all* libraries as shared. Implies
 *  `OA_<PROJECT>_DLL` for all the shared libraries.
 *
 * `OA_<PROJECT>_DLL`
 *  Define when building or using `<PROJECT>` as a shared library. These macros
 *  typically need not be used directly and are reserved for advanced usage.
 *
 * `OA_BUILD_<PROJECT>_DLL`
 *  Define *only* when building `<PROJECT>`. Implies `OA_<PROJECT>_DLL.` This
 *  is a no-op when not compiling on Windows but could be useful.
 */

// dao
// OA_DLL or OA_BUILD_DAO_DLL implies OA_DAO_DLL
#if defined(OA_DLL) || defined(OA_BUILD_DAO_DLL)
#ifndef OA_DAO_DLL
#define OA_DAO_DLL
#endif  // OA_DAO_DLL
#endif // !defined(OA_DLL) && !defined(OA_BUILD_DAO_DLL)
// building dynamic
#if defined(OA_DLL) || defined(OA_DAO_DLL)
// Windows
#if defined(_WIN32)
#if defined(OA_BUILD_DAO_DLL)
#define OA_DAO_API __declspec(dllexport)
#else
#define OA_DAO_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_DAO_DLL)
// non-Windows. if we were compiling using GCC/Clang with -fvisiblity=hidden
// then we can define OA_DAO_API here as __attribute__((visibility("default")))
#else
#define OA_DAO_API
#endif // !defined(_WIN32)
// building static
#else
#define OA_DAO_API
#endif // !defined(OA_DLL) && !defined(OA_DAO_DLL)

// enum_mappers
// OA_DLL or OA_BUILD_ENUM_MAPPERS_DLL implies OA_ENUM_MAPPERS_DLL
#if defined(OA_DLL) || defined(OA_BUILD_ENUM_MAPPERS_DLL)
#ifndef OA_ENUM_MAPPERS_DLL
#define OA_ENUM_MAPPERS_DLL
#endif  // OA_ENUM_MAPPERS_DLL
#endif  // !defined(OA_DLL) && !defined(OA_BUILD_ENUM_MAPPERS_DLL)
// building dynamic
#if defined(OA_DLL) || defined(OA_ENUM_MAPPERS_DLL)
// Windows
#if defined(_WIN32)
#if defined(OA_BUILD_ENUM_MAPPERS_DLL)
#define OA_ENUM_MAPPERS_API __declspec(dllexport)
#else
#define OA_ENUM_MAPPERS_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_ENUM_MAPPERS_DLL)
// non-Windows
#else
#define OA_ENUM_MAPPERS_API
#endif // !defined(_WIN32)
// building static
#else
#define OA_ENUM_MAPPERS_API
#endif // !defined(OA_DLL) && !defined(OA_ENUM_MAPPERS_DLL)

// helpers
// OA_DLL or OA_BUILD_HELPERS_DLL implies OA_HELPERS_DLL
#if defined(OA_DLL) || defined(OA_BUILD_HELPERS_DLL)
#ifndef OA_HELPERS_DLL
#define OA_HELPERS_DLL
#endif  // OA_HELPERS_DLL
#endif  // !defined(OA_DLL) && !defined(OA_BUILD_HELPERS_DLL)
// building dynamic
#if defined(OA_DLL) || defined(OA_HELPERS_DLL)
// Windows
#if defined(_WIN32)
#if defined(OA_BUILD_HELPERS_DLL)
#define OA_HELPERS_API __declspec(dllexport)
#else
#define OA_HELPERS_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_HELPERS_DLL)
// non-Windows
#else
#define OA_HELPERS_API
#endif // !defined(_WIN32)
// building static
#else
#define OA_HELPERS_API
#endif // !defined(OA_DLL) && !defined(OA_HELPERS_DLL)

// static_data_cache
// OA_DLL or OA_BUILD_STATIC_DATA_CACHE_DLL implies OA_STATIC_DATA_CACHE_DLL
#if defined(OA_DLL) || defined(OA_BUILD_STATIC_DATA_CACHE_DLL)
#ifndef OA_STATIC_DATA_CACHE_DLL
#define OA_STATIC_DATA_CACHE_DLL
#endif  // OA_STATIC_CACHE_DLL
#endif  // !defined(OA_DLL) && !defined(OA_BUILD_STATIC_DATA_CACHE_DLL)
// building dynamic
#if defined(OA_DLL) || defined(OA_STATIC_DATA_CACHE_DLL)
// Windows
#if defined(_WIN32)
#if defined(OA_BUILD_STATIC_DATA_CACHE_DLL)
#define OA_STATIC_DATA_CACHE_API __declspec(dllexport)
#else
#define OA_STATIC_DATA_CACHE_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_STATIC_DATA_CACHE_DLL)
// non-Windows
#else
#define OA_STATIC_DATA_CACHE_API
#endif // !defined(_WIN32)
// building static
#else
#define OA_STATIC_DATA_CACHE_API
#endif // !defined(OA_DLL) && !defined(OA_STATIC_DATA_CACHE_DLL)

// time
// OA_DLL or OA_BUILD_TIME_DLL implies OA_TIME_DLL
#if defined(OA_DLL) || defined(OA_BUILD_TIME_DLL)
#ifndef OA_TIME_DLL
#define OA_TIME_DLL
#endif  // OA_STATIC_CACHE_DLL
#endif  // !defined(OA_DLL) && !defined(OA_BUILD_TIME_DLL)
// building dynamic
#if defined(OA_DLL) || defined(OA_TIME_DLL)
// Windows
#if defined(_WIN32)
#if defined(OA_BUILD_TIME_DLL)
#define OA_TIME_API __declspec(dllexport)
#else
#define OA_TIME_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_TIME_DLL)
// non-Windows
#else
#define OA_TIME_API
#endif // !defined(_WIN32)
// building static
#else
#define OA_TIME_API
#endif // !defined(OA_DLL) && !defined(OA_TIME_DLL)

#endif  // OA_DLLEXPORT_H_
