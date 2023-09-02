/**
 * @file dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for DLL export macros
 * @copyright MIT License
 */

#ifndef OA_DLLEXPORT_H_
#define OA_DLLEXPORT_H_

// macro for global dynamic library building. this should be defined if all the
// Odin libraries are being used as dynamic, not static, libraries. the other
// OA_<PROJECT_NAME>_DLL macros provide individual control over which libraries
// are being built and used as dynamic libraries, which is advanced usage.
#ifdef OA_DLL
// dao
#ifndef OA_DAO_DLL
#define OA_DAO_DLL
#endif  // OA_DAO_DLL
// enum_mappers
#ifndef OA_ENUM_MAPPERS_DLL
#define OA_ENUM_MAPPERS_DLL
#endif  // OA_DAO_DLL
// helpers
#ifndef OA_HELPERS_DLL
#define OA_HELPERS_DLL
#endif  // OA_HELPERS_DLL
// static_data_cache
#ifndef OA_STATIC_DATA_CACHE_DLL
#define OA_STATIC_DATA_CACHE_DLL
#endif  // OA_STATIC_DATA_CACHE_DLL
// time
#ifndef OA_TIME_DLL
#define OA_TIME_DLL
#endif  // OA_TIME_DLL
#endif  // OA_DLL

// dao
#if defined(_WIN32)
// indicate that DLL is being built, this is when we want __declspec(dllexport)
// on Windows. this could also be used to set visibility back to default if we
// were compiling with GCC/Clang with -fvisibility=hidden
#if defined(OA_BUILD_DAO_DLL)
// OA_BUILD_DAO_DLL implies OA_DAO_DLL
#ifndef OA_DAO_DLL
#define OA_DAO_DLL
#endif  // OA_DAO_DLL
#define OA_DAO_API __declspec(dllexport)
#elif defined(OA_DLL) || defined(OA_DAO_DLL)
#define OA_DAO_API __declspec(dllimport)
#else
#define OA_DAO_API
#endif  // !defined(OA_BUILD_DAO_DLL) && !defined(OA_DLL) &&
        // !defined(OA_DAO_DLL)
// for non-Windows systems, no attributes are needed usually
#else
#define OA_DAO_API
#endif  // !defined(_WIN32)

// enum_mappers
#if defined(_WIN32)
#if defined(OA_BUILD_ENUM_MAPPERS_DLL)
// OA_BUILD_ENUM_MAPPERS_DLL implies OA_ENUM_MAPPERS_DLL
#ifndef OA_ENUM_MAPPERS_DLL
#define OA_ENUM_MAPPERS_DLL
#endif  // OA_ENUM_MAPPERS_DLL
#define OA_ENUM_MAPPERS_API __declspec(dllexport)
#elif defined(OA_DLL) || defined(OA_ENUM_MAPPERS_DLL)
#define OA_ENUM_MAPPERS_API __declspec(dllimport)
#else
#define OA_ENUM_MAPPERS_API
#endif  // !defined(OA_BUILD_ENUM_MAPPERS_DLL) && !defined(OA_DLL) &&
        // !defined(OA_ENUM_MAPPERS_DLL)
#else
#define OA_ENUM_MAPPERS_API
#endif  // !defined(_WIN32)

// helpers
#if defined(_WIN32)
#if defined(OA_BUILD_HELPERS_DLL)
// OA_BUILD_HELPERS_DLL implies OA_HELPERS_DLL
#ifndef OA_HELPERS_DLL
#define OA_HELPERS_DLL
#endif  // OA_HELPERS_DLL
#define OA_HELPERS_API __declspec(dllexport)
#elif defined(OA_DLL) || defined(OA_HELPERS_DLL)
#define OA_HELPERS_API __declspec(dllimport)
#else
#define OA_HELPERS_API
#endif  // !defined(OA_BUILD_HELPERS_DLL) && !defined(OA_DLL) &&
        // !defined(OA_HELPERS_DLL)
#else
#define OA_HELPERS_API
#endif  // !defined(_WIN32)

// static_data_cache
#if defined(_WIN32)
#if defined(OA_BUILD_STATIC_DATA_CACHE_DLL)
// OA_BUILD_DAO_DLL implies OA_STATIC_DATA_CACHE_DLL
#ifndef OA_STATIC_DATA_CACHE_DLL
#define OA_STATIC_DATA_CACHE_DLL
#endif  // OA_STATIC_DATA_CACHE_DLL
#define OA_STATIC_DATA_CACHE_API __declspec(dllexport)
#elif defined(OA_DLL) || defined(OA_STATIC_DATA_CACHE_DLL)
#define OA_STATIC_DATA_CACHE_API __declspec(dllimport)
#else
#define OA_STATIC_DATA_CACHE_API
#endif  // !defined(OA_BUILD_STATIC_DATA_CACHE_DLL) && !defined(OA_DLL) &&
        // !defined(OA_STATIC_DATA_CACHE_DLL)
#else
#define OA_STATIC_DATA_CACHE_API
#endif  // !defined(_WIN32)

// time
#if defined(_WIN32)
#if defined(OA_BUILD_TIME_DLL)
// OA_BUILD_TIME_DLL implies OA_TIME_DLL
#ifndef OA_TIME_DLL
#define OA_TIME_DLL
#endif  // OA_TIME_DLL
#define OA_TIME_API __declspec(dllexport)
#elif defined(OA_DLL) || defined(OA_TIME_DLL)
#define OA_TIME_API __declspec(dllimport)
#else
#define OA_TIME_API
#endif  // !defined(OA_BUILD_TIME_DLL) && !defined(OA_DLL) &&
        // !defined(OA_TIME_DLL)
#else
#define OA_TIME_API
#endif  // !defined(_WIN32)

#endif  // OA_DLLEXPORT_H_
