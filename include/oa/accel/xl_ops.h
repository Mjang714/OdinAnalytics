/**
 * @file xl_ops.h
 * @author Derek Huang
 * @brief C++ header for `XLCALL.H` type non-member operators
 * @copyright MIT License
 */

#ifndef OA_ACCEL_XL_OPS_H_
#define OA_ACCEL_XL_OPS_H_

// forward decls to avoid pulling in XLCALL.H
struct xlref12;

/**
 * Equality operator for `xlref12`.
 *
 * Result is unspecified if `a` or `b` have unspecified values.
 *
 * @param a First rectangular reference
 * @param b Second rectangular reference
 */
bool operator==(const xlref12& a, const xlref12& b) noexcept;

/**
 * Inequality operator for `xlref12`.
 *
 * Result is unspecified if `a` or `b` have unspecified values.
 *
 * @param a First rectangular reference
 * @param b Second rectangular reference
 */
bool operator!=(const xlref12& a, const xlref12& b) noexcept;

#endif  // OA_ACCEL_XL_OPS_H_
