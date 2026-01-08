/**
 * @file xl_ops.h
 * @author Derek Huang
 * @brief C++ header for `XLCALL.H` type non-member operators
 * @copyright MIT License
 */

#ifndef OA_ACCEL_XL_OPS_H_
#define OA_ACCEL_XL_OPS_H_

#include <iosfwd>

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

/**
 * Stream operator for a `xlref12`.
 *
 * For a single-cell reference, e.g. first + last rows are equal and first +
 * last columns are equal, the `xlref12` is formatted as `"(row, col)"`. For
 * other multi-cell references the format is `"(r1, c1) ... (r2, c2)"`.
 *
 * @param out Output stream
 * @param ref Excel cell reference to stream
 */
std::ostream& operator<<(std::ostream& out, const xlref12& ref);

#endif  // OA_ACCEL_XL_OPS_H_
