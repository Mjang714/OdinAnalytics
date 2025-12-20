/**
 * @file config/paths.cpp
 * @author Derek Huang
 * @brief C++ source for library path helpers
 * @copyright MIT License
 */

#include "oa/config/paths.h"

// Windows
#if defined(_WIN32)
// reduce include size
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>
// POSIX/Linux
#else
// note: required by Linux but not BSD
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif  // _GNU_SOURCE
#include <dlfcn.h>
#endif  // !defined(_WIN32)

// only need errno if compiling with _GNU_SOURCE here
#ifdef _GNU_SOURCE
#include <cerrno>
#endif  // _GNU_SOURCE
#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>

#include "oa/rtti.h"

namespace oa {
namespace config {

std::filesystem::path library_path()
{
#if defined(_WIN32)
  // get module handle for DLL
  // note: when using the debug C runtime for debug builds we currently opt to
  // build the library with a "d" suffix. this can be detected by checking if
  // _DEBUG is defined which lets us correctly conditionally compile
  // TODO: if we want to avoid hardcoding these names we can included a header
  // configured by CMake that would have the DLL name
#if defined(_DEBUG)
  auto mod = GetModuleHandleA("oa_configd");
#else
  auto mod = GetModuleHandleA("oa_config");
#endif  // _DEBUG
  if (!mod)
    throw std::system_error{
      // note: cast required to avoid narrowing conversion from DWORD
      {static_cast<int>(GetLastError()), std::system_category()},
      OA_PRETTY_FUNCTION_NAME + std::string{": unable to get module handle"}
    };
  // get absolute path name
  char name[MAX_PATH];
  auto name_len = GetModuleFileNameA(mod, name, MAX_PATH);
  // handle any errors
  if (!name_len)
    throw std::system_error{
      {static_cast<int>(GetLastError()), std::system_category()},
      OA_PRETTY_FUNCTION_NAME + std::string{": unable to get module file name"}
    };
  // done, return absolute path
  // note: using {begin, end} ctor overload since Windows XP doesn't
  // null-terminate the returned character buffer on truncation
  return {name, name + name_len};
#else
  // use address of library_path() itself to get path of oa_config
  Dl_info info;
  // note: shouldn't fail since library_path() is part of the DSO itself
  if (!dladdr(reinterpret_cast<const void*>(&library_path), &info))
    throw std::system_error{
      {errno, std::system_category()},
      OA_PRETTY_FUNCTION_NAME + std::string{": dladdr() failed"}
    };
  // return absolute path
  return info.dli_fname;
#endif  // !defined(_WIN32)
}

std::filesystem::path library_dir()
{
  return library_path().parent_path();
}

std::filesystem::path data_dir()
{
  // path to library directory
  auto lib_dir = library_dir();
  // preferred path to shared data
  auto share_dir = lib_dir.parent_path() / "share" / "OA";
  // if share_dir doesn't exist use lib_dir
  return (std::filesystem::exists(share_dir)) ? share_dir : lib_dir;
}

}  // namespace config
}  // namespace oa
