/**
 * @file fp12_view.h
 * @author Derek Huang
 * @brief C++ header for a `FP12` view type
 * @copyright MIT License
 */

#ifndef OA_ACCEL_FP12_VIEW_H_
#define OA_ACCEL_FP12_VIEW_H_

#include "oa/accel/matrix_view.h"

// forward decls to avoid pulling in XLCALL.H
struct _FP12;
using FP12 = _FP12;

namespace oa {
namespace accel {

/**
 * `FP12` view type.
 *
 * This provides a simple read-only 2D view over the Excel `FP12` type.
 */
class fp12_view : public matrix_view<const double> {
public:
  /**
   * Ctor.
   *
   * Takes a view over the `FP12` buffer.
   *
   * @param in `FP12` object
   */
  fp12_view(const FP12& in) noexcept;
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_FP12_VIEW_H_
