/**
 * @file xl_auto.cpp
 * @author Derek Huang
 * @brief C++ source for Excel XLL "auto" interface functions
 * @copyright MIT License
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include "oa/accel/oper12.h"

extern "C" {

/**
 * Excel callback to deallocate any `oper12` released back to Excel.
 *
 * @note We could make this extensible by using a class to represent add-in
 *  state like XLL+ does and then we could use the private virtual pattern to
 *  enable user-defined overrides to be run when `xlAutoFree12()` is called.
 */
__declspec(dllexport) void xlAutoFree12(xloper12* op) noexcept
{
  // note: xlbitDLLFree should already be set
  oa::accel::xloper12_free(op);
}

}  // extern "C"
