/**
 * @file xl_ops.h
 * @author Derek Huang
 * @brief C++ header for `XLCALL.H` type non-member operations
 * @copyright MIT License
 */

#ifndef OA_ACCEL_XL_OPS_H_
#define OA_ACCEL_XL_OPS_H_

#include <cstddef>
#include <iosfwd>

// forward decls to avoid pulling in XLCALL.H
struct xloper12;
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

/**
 * Stream operator for a `xloper12`.
 *
 * This uses any available `operator<<` defined for the relevant `XLCALL.H`
 * type and is called recursively for `xltypeMulti` objects. The `xltypeMulti`
 * objects will always be displayed as 2D arrays.
 *
 * @note `xltypeStr` data will be narrowed so some Unicode code points may not
 *  display as expected when written to the narrow stream.
 *
 * @par
 *
 * @note `xltypeBigData` data makes the assumption that the `XLOPER12` was
 *  zeroed before initialization and so treats zero `cbData` as an indication
 *  that the `HANDLE` should be formatted instead of the `BYTE*` in the union.
 *
 * @par
 *
 * @todo `xltypeFlow` is not yet supported and returns just the type + address.
 *
 * @param out Output stream
 * @param op `XLOPER12` to write to stream
 */
std::ostream& operator<<(std::ostream& out, const xloper12& op);

namespace oa {
namespace accel {

/**
 * Return the number of rows in the `xltypeMulti` array.
 *
 * Zero is returned if the `XLOPER12` is not a `xltypeMulti`.
 *
 * @param op `XLOPER12` to get `xltypeMulti` rows for
 */
std::size_t rows(const xloper12& op) noexcept;

/**
 * Return the number of columns in the `xltypeMulti` array.
 *
 * Zero is returned if the `XLOPER12` is not a `xltypeMulti`.
 *
 * @param op `XLOPER12` to get `xltypeMulti` columns for
 */
std::size_t cols(const xloper12& op) noexcept;

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_XL_OPS_H_
