/**
 * @file dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for DLL export macros
 * @copyright MIT License
 */

#ifndef OA_DLLEXPORT_H_
#define OA_DLLEXPORT_H_

// note: we can change checking just defined(_WIN32) to also include checking
// defined(OA_DLL) if we want static default, shared optional

// dao
#if defined(_WIN32)
// indicate that DLL is being built, this is when we want __declspec(dllexport)
// on Windows. this could also be used to set visibility back to default if we
// were compiling with GCC/Clang with -fvisibility=hidden
#if defined(OA_BUILD_DAO_DLL)
#define OA_DAO_API __declspec(dllexport)
#else
#define OA_DAO_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_DAO_DLL)
// for non-Windows systems, no attributes are needed usually
#else
#define OA_DAO_API
#endif  // !defined(_WIN32)

// enum_mappers
#if defined(_WIN32)
#if defined(OA_BUILD_ENUM_MAPPERS_DLL)
#define OA_ENUM_MAPPERS_API __declspec(dllexport)
#else
#define OA_ENUM_MAPPERS_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_ENUM_MAPPERS_DLL)
#else
#define OA_ENUM_MAPPERS_API
#endif  // !defined(_WIN32)

// helpers
#if defined(_WIN32)
#if defined(OA_BUILD_HELPERS_DLL)
#define OA_HELPERS_API __declspec(dllexport)
#else
#define OA_HELPERS_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_HELPERS_DLL)
#else
#define OA_HELPERS_API
#endif  // !defined(_WIN32)

// static_data_cache
#if defined(_WIN32)
#if defined(OA_BUILD_STATIC_DATA_CACHE_DLL)
#define OA_STATIC_DATA_CACHE_API __declspec(dllexport)
#else
#define OA_STATIC_DATA_CACHE_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_STATIC_DATA_CACHE_DLL)
#else
#define OA_STATIC_DATA_CACHE_API
#endif  // !defined(_WIN32)

// time
#if defined(_WIN32)
#if defined(OA_BUILD_TIME_DLL)
#define OA_TIME_API __declspec(dllexport)
#else
#define OA_TIME_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_TIME_DLL)
#else
#define OA_TIME_API
#endif  // !defined(_WIN32)

#endif  // OA_DLLEXPORT_H_
