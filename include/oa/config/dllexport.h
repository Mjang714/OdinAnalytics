/**
 * @file config/dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for `oa_config` symbol visibility macros
 * @copyright MIT License
 */

#ifndef OA_CONFIG_DLLEXPORT_H_
#define OA_CONFIG_DLLEXPORT_H_

// note: oa_config always built shared so OA_DLL has no effect. but for
// consistency with the rest of the libraries we always define OA_CONFIG_DLL
#ifndef OA_CONFIG_DLL
#define OA_CONFIG_DLL
#endif  // OA_CONFIG_DLL

// Windows
#if defined(_WIN32)
#if defined(OA_BUILD_CONFIG_DLL)
#define OA_CONFIG_API __declspec(dllexport)
#else
#define OA_CONFIG_API __declspec(dllimport)
#endif  // !defined(OA_BUILD_CONFIG_DLL)
// GCC/Clang
#elif defined(__GNUC__)
#define OA_CONFIG_API __attribute__((visibility("default")))
// other compilers
#else
#define OA_CONFIG_API
#endif  // !defined(_WIN32)

#endif  // OA_CONFIG_DLLEXPORT_H_
