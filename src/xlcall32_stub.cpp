/**
 * @file xlcall32_stub.cpp
 * @author Derek Huabg
 * @brief C++ XLCALL32.DLL stub implementtation
 * @copyright MIT License
 *
 * This file is designed to mock out the functions exported by `XLCALL32.DLL`:
 *
 * @code
 * File Type: DLL
 *
 * Section contains the following exports for XLCall32.dll
 *
 *     00000000 characteristics
 *     69167869 time date stamp Thu Nov 13 19:31:37 2025
 *         0.00 version
 *            1 ordinal base
 *            4 number of functions
 *            4 number of names
 *
 *     ordinal hint RVA      name
 *
 *           2    0 00001080 Excel4
 *           3    1 000011B0 Excel4v
 *           4    2 00001220 LPenHelper
 *           1    3 00001070 XLCallVer
 * @endcode
 *
 * This enables us to run applications that link against `XLCALL32.LIB` without
 * requiring locating a true Excel installation's `XLCALL32.DLL` for testing.
 *
 * To distinguish this from a "real" Excel `XLCALL32.DLL` at runtime one can
 * either check if `GetProcAddress(h, "xlcall32_is_stub")` is not `NULL`, where
 * `h` is the `GetModuleHandleA("XLCALL32")` handle, or from code that includes
 * `XLCALL.H`, call `XLCallVer()` and check if the return value is zero.
 *
 * The benefit of adding the `xlcall32_is_stub()` function is primarily to make
 * it easy to determine that an `XLCALL32.DLL` is this stub implementation via
 * usage of `dumpbin /exports XLCALL32.DLL`.
 *
 * @note This file must be compiled with the provided `.def` file and linked
 *  into a DLL called `XLCALL32.DLL`. We cannot `__declspec(dllexport)` because
 *  MSVC will complain about a change in linkage and because the `.def` file
 *  is the only way we can export names at specific ordinals.
 *
 * @todo Mock implementations may be provided to simulate callback
 *  functionality, e.g. so `xlFree` and `xlGetName` can work.
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

extern "C" {

/**
 * `Excel4()` stub that always returns `xlretFailed`.
 */
int __cdecl Excel4(int, LPXLOPER, int, ...)
{
  return xlretFailed;
}

/**
 * `Excel4v()` stub that always returns `xlretFailed`.
 */
int pascal Excel4v(int, LPXLOPER, int, LPXLOPER[])
{
  return xlretFailed;
}

/**
 * `LPenHelper()` stub that always returns zero.
 */
long pascal LPenHelper(int, VOID*)
{
  return 0;
}

/**
 * `XLCallVer()` stub that always returns zero.
 */
int pascal XLCallVer()
{
  return 0;
}

/**
 * Return `true` to indicate this DLL is a `XLCALL32` stub implementation.
 *
 * @note We can use `__declspec(dllexport)` here since this is our function.
 */
__declspec(dllexport) bool xlcall32_is_stub() noexcept
{
  return true;
}

}  // extern "C"
