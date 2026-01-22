/**
 * @file oper12_row_view.h
 * @author Derek Huang
 * @brief C++ header for a view type over a row of an `xltypeMulti` operand
 * @copyright MIT License
 */

#ifndef OA_ACCEL_OPER12_ROW_VIEW_H_
#define OA_ACCEL_OPER12_ROW_VIEW_H_

#include <cstddef>

// forward decl to avoid pulling in XLCALL.H
struct xloper12;

namespace oa {
namespace accel {

// forward decl to break dependency cycle
class oper12_view;

// TODO: document

/**
 * Excel 12 `xltypeMulti` type operand array row read-only view class.
 *
 * This provides a lightweight view of the array member of a `XLOPER12` of type
 * `xltypeMulti`. Like with the `oper12_view`, the `oper12_row_view` is a
 * read-only view type because without a priori knowledge one cannot tell the
 * ownership strategy of any auxiliary memory owned by a viewed `XLOPER12`.
 */
class oper12_row_view {
public:
  /**
   * Ctor.
   *
   * @param buf Buffer of `xloper12` instances
   * @param size Number of `xloper12` instances in row view
   */
  oper12_row_view(const xloper12* buf, std::size_t size) noexcept;

  /**
   * Return an `oper12_view` to the `i`th `XLOPER12` in the row view.
   *
   * Behavior is undefined if indexing out of array bounds is performed.
   *
   * @param i Index of desired `XLOPER12`
   */
  oper12_view operator[](std::size_t i) const noexcept;

  /**
   * Return the number of elements in the `XLOPER12` row view.
   */
  std::size_t size() const noexcept;

  // TODO: add iterator type that yields oper12_view
  // TODO: add vector<T>() overload for converting to a std::vector<T>

private:
  const xloper12* buf_;
  std::size_t size_;
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_OPER12_ROW_VIEW_H_
