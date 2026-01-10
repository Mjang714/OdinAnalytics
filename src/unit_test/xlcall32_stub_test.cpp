/**
 * @file xlcall32_stub_test.cpp
 * @author Derek Huang
 * @brief C++ unit tests for the stub `XLCALL32` DLL
 * @copyright MIT License
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <system_error>

#include <gtest/gtest.h>

namespace {

/**
 * Helper macro to get a `XLCALL32` proc address + check if it is not `NULL`.
 *
 * This can only be used from within the `StubXlCall32Test`.
 *
 * @param out Name to bind new function pointer to
 * @param name Procedure name
 * @param type Function type to cast `FARPROC` to
 */
#define GTEST_ASSERT_PROC_EXISTS(out, name, type) \
  auto out = [&] { return reinterpret_cast<type>(xlcall32_proc(#name)); }(); \
  ASSERT_TRUE(out) << #name "() missing: " << \
    std::system_category().message(static_cast<int>(GetLastError()))

/**
 * Test fixture for stub `XLCALL32` DLL tests.
 */
class StubXlCall32Test : public ::testing::Test {
protected:
  /**
   * Obtain the module handle for `XLCALL32`.
   *
   * This should not return `nullptr` and is required by most of the tests.
   *
   * @note The returned `HMODULE` does not increment the module ref count.
   */
  static auto xlcall32_handle()
  {
    auto hnd = GetModuleHandleA("XLCALL32");
    if (!hnd)
      throw std::system_error{
        {static_cast<int>(GetLastError()), std::system_category()},
        "GetModuleHandleA() failed"
      };
    return hnd;
  }

  /**
   * Obtain the function pointer for the specified function.
   *
   * On error `nullptr` is returned so check `GetLastError()`.
   *
   * @param name Name of the DLL procedure to get a pointer to
   */
  static auto xlcall32_proc(const char* name)
  {
    return GetProcAddress(xlcall32_handle(), name);
  }
};

/**
 * Test that `Excel4()` is available and returns `xlretFailed`.
 */
TEST_F(StubXlCall32Test, Excel4Test)
{
  GTEST_ASSERT_PROC_EXISTS(
    proc,
    Excel4,
    int (__cdecl *)(int, LPXLOPER, int, ...)
  );
  // both function pointer + name should work
  XLOPER res;
  EXPECT_EQ(xlretFailed, proc(xlSheetId, &res, 1, nullptr));
  EXPECT_EQ(xlretFailed, Excel4(xlSheetId, &res, 1, nullptr));
}

/**
 * Test that `Excel4v()` is available and returns `xlretFailed`.
 */
TEST_F(StubXlCall32Test, Excel4vTest)
{
  GTEST_ASSERT_PROC_EXISTS(
    proc,
    Excel4v,
    int (pascal *)(int, LPXLOPER, int, LPXLOPER[])
  );
  // both function pointer + name should work
  XLOPER res;
  LPXLOPER args[] = {nullptr};
  EXPECT_EQ(xlretFailed, proc(xlSheetId, &res, 1, args));
  EXPECT_EQ(xlretFailed, Excel4v(xlSheetId, &res, 1, args));
}

/**
 * Test that `LPenHelper()` is available and returns zero.
 */
TEST_F(StubXlCall32Test, LPenHelperTest)
{
  GTEST_ASSERT_PROC_EXISTS(proc, LPenHelper, long (pascal *)(int, VOID*));
  // both function pointer + name should work
  EXPECT_EQ(0, proc(xlGetFmlaInfo, nullptr));
  EXPECT_EQ(0, LPenHelper(xlGetFmlaInfo, nullptr));
}

/**
 * Test that `XLCallVer()` is available and returns zero.
 */
TEST_F(StubXlCall32Test, XLCallVerTest)
{
  GTEST_ASSERT_PROC_EXISTS(proc, XLCallVer, int (pascal *)());
  // both function pointer + name should work
  EXPECT_EQ(0, proc());
  EXPECT_EQ(0, XLCallVer());
}

/**
 * Test that `xlcall32_is_stub()` is available and returns `true`.
 *
 * This is the true way to distinguish if the loaded `XLCALL32.DLL` is our stub
 * implementation or not as the real `XLCALL32.DLL` doesn't export this.
 */
TEST_F(StubXlCall32Test, StubProcTest)
{
  GTEST_ASSERT_PROC_EXISTS(proc, xlcall32_is_stub, bool (*)() noexcept);
  ASSERT_TRUE(proc()) << "xlcall32_is_stub() should return true";
}

/**
 * Test that `Excel12()` returns zero.
 *
 * Although not implemented by the stub `XLCALL32` DLL as it is implemented in
 * `XLCALL.CPP`, if not running in Excel, `xlretFailed` should be returned.
 */
TEST_F(StubXlCall32Test, Excel12Test)
{
  XLOPER12 res;
  EXPECT_EQ(xlretFailed, Excel12(xlSheetId, &res, 1, nullptr));
}

/**
 * Test that `Excel12v()` returns zero
 *
 * This should also return `xlretFailed` like `Excel12()`.
 */
TEST_F(StubXlCall32Test, Excel12vTest)
{
  XLOPER12 res;
  LPXLOPER12 args[] = {nullptr};
  EXPECT_EQ(xlretFailed, Excel12v(xlSheetId, &res, 1, args));
}

// clean up (especially since we do unity builds)
#undef GTEST_ASSERT_PROC_EXISTS

}  // namespace
