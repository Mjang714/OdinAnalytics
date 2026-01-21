/**
 * @file xl_conv.h
 * @author Derek Huang
 * @brief C++ header for `XLCALL.H` type converters
 * @copyright MIT License
 */

#ifndef OA_ACCEL_XL_CONV_H_
#define OA_ACCEL_XL_CONV_H_

#include <concepts>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// forward decl to avoid pulling in XLCALL.H
struct xloper12;

namespace oa {
namespace accel {

/**
 * Get the `double` value stored in a `XLOPER12` of type `xltypeNum`.
 *
 * Conversions are done if the type is `xltypeBool` or `xltypeInt` and an
 * exception is otherwise thrown to indicate conversion failure.
 *
 * @param op `XLOPER12` to convert
 */
double as_double(const xloper12& op);

/**
 * Get the `double` value stored in a `XLOPER12` of type `xltypeNum` as float.
 *
 * This simply casts the `as_double()` value into a `float`.
 *
 * @param op `XLOPER12` to convert
 */
float as_float(const xloper12& op);

/**
 * Get the string stored in a `XLOPER12` of type `xltypeStr` as a string.
 *
 * Conversions are done if the type is `xltypeNum`, `xltypeBool`, `xltypeErr`,
 * and `xltypeInt`, with exception thrown otherwise.
 *
 * @note This does *not* provide the same result as `operator<<` as only a few
 *  conversions are allowed due to different intended semantics.
 *
 * @param op `XLOPER12` to convert
 */
std::string as_string(const xloper12& op);

/**
 * Get the string stored in a `XLOPER12` of type `xltypeStr` as a wide string.
 *
 * Conversions are done if the type is `xltypeNum`, `xltypeBool`, `xltypeErr`,
 * and `xltypeInt`, with exception thrown otherwise.
 *
 * @param op `XLOPER12` to convert
 */
std::wstring as_wstring(const xloper12& op);

/**
 * Get a view of the wide string stored in a `XLOPER12` of type `xltypeStr`.
 *
 * This provides a direct view into the `XLOPER12` wide character buffer and
 * therefore saves a copy if the string content is rather long. An exception is
 * thrown if the `XLOPER12` is not a `xltypeStr` as converting is not possible.
 *
 * @param op `XLOPER12` to get view of
 */
std::wstring_view as_wstring_view(const xloper12& op);

/**
 * `xltypeMulti` conversion options.
 */
class multi_conv_options {
public:
  using self = multi_conv_options;

  /**
   * Indicate whether input must be `xltypeMulti` or not.
   *
   * @param v `true` to require only `xltypeMulti`
   */
  self& strict(bool v) noexcept;

  /**
   * Indicate whether input is required to be `xltypeMulti` or not.
   */
  bool strict() const noexcept;

  /**
   * Indicate whether `xltypeMulti` input shape must be a vector.
   *
   * This means that at least one dimension, row or column, is 1. Note that
   * passing `false` also makes `row_vector()` and `col_vector()` be `false`.
   *
   * @param v `true` to require `xltypeMulti` input to be a vector
   */
  self& vector(bool v) noexcept;

  /**
   * Indicate whether `xltypeMulti` input shape must be a vector.
   */
  bool vector() const noexcept;

  /**
   * Indicate whether `xltypeMulti` input shape must be a row vector.
   *
   * Setting `row_vector()` also directly sets the value of `vector()`.
   *
   * @param v `true` to require `xltypeMulti` input to be one row
   */
  self& row_vector(bool v) noexcept;

  /**
   * Indicate whether `xltypeMulti` input shape must be a row vector.
   */
  bool row_vector() const noexcept;

  /**
   * Indicate whether `xltypeMulti` input shape must be a column vector.
   *
   * Setting `col_vector()` also directly sets the value of `vector()`.
   *
   * @param v `true` to require `xltypeMulti` input to be one column
   */
  self& col_vector(bool v) noexcept;

  /**
   * Indicate whether `xltypeMulti` input shape must be a column vector.
   */
  bool col_vector() const noexcept;

private:
  bool strict_{};
  bool vector_{};
  bool row_vector_{};
  bool col_vector_{};
};

/**
 * Fill a double buffer from the `xltypeMulti` array data of a `XLOPER12`.
 *
 * This flattens the `xltypeMulti` values in row-major order and will throw an
 * exception if the `XLOPER12` is not a `xltypeMulti` or if one of the members
 * of the `xltypeMulti` array cannot be converted using `as_double()`.
 *
 * Note that `XLOPER12` of type `xltypeNum` or `xltypeInt` are treated as
 * 1-element arrays. It is assumed the provided buffer is large enough to fit
 * all the elements of the `xltypeMulti`, i.e. it can hold at least
 * `array.rows * array.columns` elements.
 *
 * @note Typically this need not be used directly unless implementing an
 *  `xloper12_converter<T>` specialization for a custom container/matrix type.
 *  In that case, this function is useful, as it is allocator-agnostic.
 *
 * @param out Output buffer
 * @param op `XLOPER12` to convert
 * @param opts Additional `xltypeMulti` conversion options
 */
void as(double* out, const xloper12& op, const multi_conv_options& opts = {});

/**
 * Fill a float buffer from the `xltypeMulti` array data of a `XLOPER12`.
 *
 * This has the same semantics as the `as(double*, const xloper12&)` overload.
 *
 * @param out Output buffer
 * @param op `XLOPER12` to convert
 * @param opts Additional `xltypeMulti` conversion options
 */
void as(float* out, const xloper12& op, const multi_conv_options& opts = {});

/**
 * Get a vector of doubles from the `xltypeMulti` array data of a `XLOPER12`.
 *
 * This flattens the `xltypeMulti` values in row-major order and will throw an
 * exception if the `XLOPER12` is not a `xltypeMulti` or if one of the members
 * of the `xltypeMulti` array cannot be converted to `double`.
 *
 * Note that `XLOPER12` of type `xltypeNum` or `xltypeInt` are converted to
 * 1-element vectors as an easy-to-implement convenience.
 *
 * @param op `XLOPER12` to convert
 * @param opts Additional `xltypeMulti` conversion options
 */
std::vector<double>
as_double_vector(const xloper12& op, const multi_conv_options& opts = {});

/**
 * Get a vector of floats from the `xltypeMulti` array data of a `XLOPER12`.
 *
 * This has the semantics of `as_double_vector()`.
 *
 * @param op `XLOPER12` to convert
 * @param opts Additional `xltypeMulti` conversion options
 */
std::vector<float>
as_float_vector(const xloper12& op, const multi_conv_options& opts = {});

/**
 * `XLOPER12` converter template.
 *
 * Specializations can be provided for user-defined types. Each must provide an
 * `operator()` that has the following declaration:
 *
 * @code{.cc}
 * T operator()(const xloper12&) const;
 * @endcode
 *
 * A more rigorous concept is defined in `xloper12_convertible`. Note that
 * specific specializations may provide additional arguments to specify how
 * exactly to perform the conversion for more complex types.
 *
 * @tparam T type
 */
template <typename T>
struct xloper12_converter {};

/**
 * Concept for a C++ type with an appropriate `xloper12_converter`.
 *
 * @tparam T type
 */
template <typename T>
concept xloper12_convertible = requires(xloper12_converter<T> f, xloper12 v) {
  { f(v) } -> std::same_as<T>;
};

/**
 * Obtain the value stored in the `XLOPER12` as the given C++ type.
 *
 * Depending on the implementation function some conversions are allowed.
 *
 * @tparam T Target type
 * @tparam Ts Additional `xloper12_converter<T>::operator()` arguments
 *
 * @param op `XLOPER12` to convert
 * @param args Additional `xloper12_converter<T>::operator()` arguments
 */
template <xloper12_convertible T, typename... Ts>
T as(const xloper12& op, Ts&&... args)
{
  return xloper12_converter<T>{}(op, std::forward<Ts>(args)...);
}

// double specialization
template <>
struct xloper12_converter<double> {
  auto operator()(const xloper12& op) const { return as_double(op); }
};

// float specialization
template <>
struct xloper12_converter<float> {
  auto operator()(const xloper12& op) const { return as_float(op); }
};

// std::string specialization
template <>
struct xloper12_converter<std::string> {
  auto operator()(const xloper12& op) const { return as_string(op); }
};

// std::wstring specialization
template <>
struct xloper12_converter<std::wstring> {
  auto operator()(const xloper12& op) const { return as_wstring(op); }
};

// std::wstring_view specialization
template <>
struct xloper12_converter<std::wstring_view> {
  auto operator()(const xloper12& op) const { return as_wstring_view(op); }
};

// std::vector<double> specialization
template <>
struct xloper12_converter<std::vector<double>> {
  auto operator()(const xloper12& op, const multi_conv_options& opts = {}) const
  {
    return as_double_vector(op, opts);
  }
};

// std::vector<float> specialization
template <>
struct xloper12_converter<std::vector<float>> {
  auto operator()(const xloper12& op, const multi_conv_options& opts = {}) const
  {
    return as_float_vector(op, opts);
  }
};

}  // namespace oa
}  // namespace oa

#endif  // OA_ACCEL_XL_CONV_H_
