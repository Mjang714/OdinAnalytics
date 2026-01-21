/**
 * @file fp12_view.cpp
 * @author Derek Huang
 * @brief C++ source for a `FP12` view type
 * @copyright MIT License
 */

#include "oa/accel/fp12_view.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

namespace oa {
namespace accel {

fp12_view::fp12_view(const FP12& in) noexcept
  : matrix_view{
      in.array,
      // note: cast to avoid C2398 and appease compiler
      static_cast<std::size_t>(in.rows),
      static_cast<std::size_t>(in.columns)
    }
{}

}  // namespace accel
}  // namespace oa
