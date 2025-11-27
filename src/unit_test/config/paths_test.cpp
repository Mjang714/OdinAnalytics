/**
 * @file config/paths_test.cpp
 * @author Derek Huang
 * @brief config/paths.h unit tests
 * @copyright MIT License
 */

#include "oa/config/paths.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>
#else
#include <unistd.h>  // for readlink()
#endif  // !defined(_WIN32)

// again, only need errno when not on Windows
#ifdef _GNU_SOURCE
#include <cerrno>
#endif  // _GNU_SOURCE
#include <filesystem>
#include <system_error>

#include <gtest/gtest.h>

namespace {

/**
 * Test fixture for `oa_config` path helpers tests.
 */
class ConfigPathsTest : public ::testing::Test {
protected:
  /**
   * Return the directory that the currently running program is in.
   *
   * On Windows this uses the standard `GetModuleHandleA()` and
   * `GetModuleFileNameA()` pair and uses `readlink()` on Linux.
   */
  static auto self_dir()
  {
#if defined(_WIN32)
    // note: using nullptr gives us handle for current process
    auto mod = GetModuleHandleA(nullptr);
    if (!mod)
      throw std::system_error{
        // again, need cast as braced-init-list disallows narrowing
        {static_cast<int>(GetLastError()), std::system_category()},
        "GetModuleHandleA() failed"
      };
    // get path to current exe
    // note: not explicitly supporting long paths here
    char path[MAX_PATH];
    auto path_len = GetModuleFileNameA(mod, path, MAX_PATH);
    if (!path_len)
      throw std::system_error{
        {static_cast<int>(GetLastError()), std::system_category()},
        "GetModuleFIleNameA() failed"
      };
#else
    // use hardcoded Linux PATH_MAX. this is fine for stack allocation since
    // typically Linux stack size is 8M unless changed using ulimit
    char path[4096];
    // note: assumes that we can use /proc/self which is Linux-specific
    auto path_len = readlink("/proc/self/exe", path, sizeof path);
    if (path_len < 0)
      throw std::system_error{{errno, std::system_category()}, "readlink()"};
#endif  // !defined(_WIN32)
    // return parent path
    // note: readlink does *not* null terminate and GetModuleFileName won't
    // null terminate on Windows XP so we just share the same solution
    return std::filesystem::path{path, path + path_len}.parent_path();
  }
};

/**
 * Test that the library directory is as expected.
 *
 * In the build tree all binaries are output in the same directory.
 */
TEST_F(ConfigPathsTest, LibraryDirTest)
{
  EXPECT_EQ(self_dir(), oa::config::library_dir());
}

/**
 * Test that the data directory is as expected.
 *
 * The unit test runner isn't installed so `share/oa` is not expected.
 */
TEST_F(ConfigPathsTest, DataDirTest)
{
  EXPECT_EQ(self_dir(), oa::config::data_dir());
}

}  // namespace
