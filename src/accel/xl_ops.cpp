/**
 * @file xl_ops.cpp
 * @author Derek Huang
 * @brief C++ source for `XLCALL.H` type non-member operators
 * @copyright MIT License
 */

#include "oa/accel/xl_ops.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

////////////////////////////////////////////////////////////////////////////////
// xlref12                                                                    //
////////////////////////////////////////////////////////////////////////////////

bool operator==(const xlref12& a, const xlref12& b) noexcept
{
  return (
    a.rwFirst == b.rwFirst &&
    a.rwLast == b.rwLast &&
    a.colFirst == b.colFirst &&
    a.colLast == b.colLast
  );
}

bool operator!=(const xlref12& a, const xlref12& b) noexcept
{
  return !(a == b);
}
