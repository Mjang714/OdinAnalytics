/**
 * @file oper12_view.h
 * @author Derek Huang
 * @brief C++ header for a view type over the Excel 12 fundamental data type
 * @copyright MIT License
 */

#ifndef OA_ACCEL_OPER12_VIEW_H_
#define OA_ACCEL_OPER12_VIEW_H_

#include <optional>
#include <utility>

#include "oa/accel/enums.h"
#include "oa/accel/xl_conv.h"

// forward decl to avoid pulling in XLCALL.H
struct xloper12;

namespace oa {
namespace accel {

// forward decl to break dependency cycle
class oper12_row_view;

/**
 * Excel 12 fundamental data type "operand" read-only view class.
 *
 * This provides a lightweight view type over an opaque `XLOPER12` that can be
 * used to obtain information about the `XLOPER12` and perform conversions to
 * obtain C++ values. Interop with Excel SDK C functions is *not* the intention
 * of this type; it is for taking a view of an `XLOPER12` that may be provided
 * from Excel or is owned by an `oper12` allocated from C++ code. Therefore, we
 * do not want to enable modification of the underlying `XLOPER12` as it is
 * difficult to know if there is auxiliary memory associated with it.
 *
 * @note Although the `oper12_view` shares some member functions with the
 *  `oper12` we do not provide any common base class to strengthen the
 *  distinction between the two types. The `oper12` can implicitly convert to
 *  the `oper12_view` for applications where only a view is needed.
 */
class oper12_view {
public:
  /**
   * Default ctor.
   *
   * Constructs an empty view that does not refer to an existing `XLOPER12`.
   */
  oper12_view() noexcept;

  /**
   * Ctor.
   *
   * If `value` is `nullptr` then the `oper12_view` is considered empty.
   *
   * @param value `XLOPER12` to take a view of
   */
  oper12_view(const xloper12* value) noexcept;

  /**
   * Indicate if the `oper12` points to an `XLOPER12`.
   *
   * This returns `true` when the `value_` pointer is `nullptr`.
   */
  bool operator!() const noexcept;

  /**
   * Return the type enumeration corresponding to the `XLOPER12` type.
   */
  xltype type() const noexcept;

  /**
   * Return the `XLOPER12` error value if `type()` is `xltype::err`.
   *
   * If the `XLOPER12` does not contain an error the optional is empty.
   */
  std::optional<xlerr> error() const noexcept;

  /**
   * Obtain a view over the `i`th `XLOPER12` row in a `xltypeMulti`.
   *
   * Behavior is undefined if the viewed `XLOPER12` is not a `xltypeMulti` or
   * if indexing out of array bounds is performed.
   *
   * @param i Index of desired `XLOPER12` row
   */
  oper12_row_view operator[](std::size_t i) const noexcept;

  /**
   * Obtain a view over the `i`th `XLOPER12` in a `xltypeMulti`.
   *
   * Behavior is undefined if the viewed `XLOPER12` is not a `xltypeMulti` or
   * if indexing out of array bounds is performed.
   *
   * @param i Index of desired `XLOPER12`
   */
  oper12_view operator()(std::size_t i) const noexcept;

  /**
   * Obtain a view over the given `XLOPER12` in a `xltypeMulti`.
   *
   * Behavior is undefined if the viewed `XLOPER12` is not a `xltypeMulti` or
   * if indexing out of array bounds is performed.
   *
   * @param i Row index of desired `XLOPER12`
   * @param j Col index of desired `XLOPER12`
   */
  oper12_view operator()(std::size_t i, std::size_t j) const noexcept;

  /**
   * Return the number of rows in the `xltypeMulti` array.
   *
   * Zero is returned if the viewed `XLOPER12` is not a `xltypeMulti`.
   */
  std::size_t rows() const noexcept;

  /**
   * Return the number of columns in the `xltypeMulti` array.
   *
   * Zero is returned if the viewed `XLOPER12` is not a `xltypeMulti`.
   */
  std::size_t cols() const noexcept;

  /**
   * Return the number of elements in the `xltypeMulti` array.
   *
   * Zero is returned if the viewed `XLOPER12` is not a `xltypeMulti`.
   */
  std::size_t size() const noexcept;

  /**
   * Obtain the `XLOPER12` value as the specified C++ type.
   *
   * This is implemented via the `xloper12_converter<T>` templates and
   * depending on the specialization may take additional arguments.
   *
   * @tparam T C++ type with an `xloper12_converter<T>` specialization
   * @tparam Ts Additional `xloper12_converter<T>::operator()` arguments
   */
  template <xloper12_convertible T, typename... Ts>
  T as(Ts&&... args) const
  {
    return xloper12_converter<T>{}(*value_, std::forward<Ts>(args)...);
  }

private:
  const xloper12* value_;
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_OPER12_VIEW_H_
