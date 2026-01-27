/**
 * @file xl_conv.h
 * @author Derek Huang
 * @brief C++ header for `XLCALL.H` type converters
 * @copyright MIT License
 */

#ifndef OA_ACCEL_XL_CONV_H_
#define OA_ACCEL_XL_CONV_H_

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "oa/accel/enums.h"

// forward decl to avoid pulling in XLCALL.H
struct xloper12;

namespace oa {
namespace accel {

// forward decl for xloper12_converter<std::tuple<Ts...>>
class oper12_view;

namespace detail {

/**
 * Tag type to indicate that strict Excel to C++ type conversion is required.
 *
 * This is used through `strict` to select supported conversion function
 * overloads that disallow some common type conversions. For example,
 * `as_int()` can convert an `xltypeNum` value, which is a `double`, to an
 * `int` in C++, truncating from `double` to `int`. If this conversion is not
 * desired the stricter overload can be selected using `strict`.
 */
struct strict_tag {};

}  // namespace detail

/**
 * Tag global to indicate exact Excel to C++ type conversion is required.
 *
 * This selects supported conversion function overloads that are stricter, i.e.
 * which disallow some common type conversions.
 */
inline constexpr detail::strict_tag strict;

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
 * Get the `double` value stored in a `XLOPER12` of type `xltypeNum`.
 *
 * This conversion is strict and requires the input is of type `xltypeNum`.
 *
 * @param op `XLOPER12` to convert
 */
double as_double(const xloper12& op, detail::strict_tag);

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
 * Get the string stored in a `XLOPER12` of type `xltypeStr` as a string.
 *
 * This conversion is strict and requires the input is of type `xltypeStr`.
 *
 * @param op `XLOPER12` to convert
 */
std::string as_string(const xloper12& op, detail::strict_tag);

/**
 * Get the string stored in a `XLOPER12` of type `xltypeStr` as a wide string.
 *
 * Conversions are done if the type is `xltypeNum`, `xltypeBool`, `xltypeErr`,
 * and `xltypeInt`, with exception thrown otherwise.
 *
 * @note This does *not* provide the same result as `operator<<` as only a few
 *  conversions are allowed due to different intended semantics.
 *
 * @param op `XLOPER12` to convert
 */
std::wstring as_wstring(const xloper12& op);

/**
 * Get the string stored in a `XLOPER12` of type `xltypeStr` as a wide string.
 *
 * This conversion is strict and requires the input is of type `xltypeStr`.
 *
 * @param op `XLOPER12` to convert
 */
std::wstring as_wstring(const xloper12& op, detail::strict_tag);

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
 * Get the boolean value stored in a `XLOPER12` of type `xltypeBool`.
 *
 * Converting from a `xltypeInt` is allowed and an exception is otherwise
 * thrown to indicate conversion failure.
 *
 * @param op `XLOPER12` to convert
 */
bool as_bool(const xloper12& op);

/**
 * Get the boolean value stored in a `XLOPER12` of type `xltypeBool`.
 *
 * This conversion is strict and requires the input is of type `xltypeBool`.
 *
 * @param op `XLOPER12` to convert
 */
bool as_bool(const xloper12& op, detail::strict_tag);

/**
 * Get the integer value stored in a `XLOPER12` of type `xltypeInt`.
 *
 * Converting from a `xltypeBool` and `xltypeNum` is allowed and an exception
 * is otherwise thrown to indicate conversion failure.
 *
 * @param op `XLOPER12` to convert
 */
int as_int(const xloper12& op);

/**
 * Get the integer value stored in a `XLOPER12` of type `xltypeInt`.
 *
 * This conversion is strict and requires the input is of type `xltypeInt`.
 *
 * @note This is not quite useful in practice as numeric values always come
 *  from Excel as `xltypeNum` and booleans will come as `xltypeBool`.
 *
 * @param op `XLOPER12` to convert
 */
int as_int(const xloper12& op, detail::strict_tag);

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
 * Some conversions should be alloed where appropriate. Therefore, it is also
 * recommended that the following `operator()` is supported to provide stricter
 * conversion semantics, as this is often desirable:
 *
 * @code{.cc}
 * T operator()(const xloper12&, detail::strict_tag) const;
 * @endcode
 *
 * A more rigorous concept is defined in `xloper12_convertible`. Note that
 * specific specializations may provide additional arguments or `operator()`
 * overloads to specify how exactly to perform the conversion for more complex
 * types or for particular scenarios, e.g. when strict conversion is desired.
 *
 * @tparam T type
 */
template <typename T>
struct xloper12_converter {};

/**
 * Concept for a C++ type with an appropriate `xloper12_converter`.
 *
 * @tparam T Target C++ type
 * @tparam Ts Additional converter arguments
 */
template <typename T, typename... Ts>
concept xloper12_convertible =
  requires(xloper12_converter<T> f, xloper12 v, Ts... args) {
    { f(v, args...) } -> std::same_as<T>;
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
template <typename T, typename... Ts>
requires (xloper12_convertible<T, Ts...>)
T as(const xloper12& op, Ts&&... args)
{
  return xloper12_converter<T>{}(op, std::forward<Ts>(args)...);
}

// double specialization
template <>
struct xloper12_converter<double> {
  // non-strict overload
  auto operator()(const xloper12& op) const
  {
    return as_double(op);
  }

  // strict overload
  auto operator()(const xloper12& op, detail::strict_tag tag) const
  {
    return as_double(op, tag);
  }
};

// float specialization
template <>
struct xloper12_converter<float> {
  // non-strict overload
  auto operator()(const xloper12& op) const
  {
    return static_cast<float>(as_double(op));
  }

  // strict overload
  auto operator()(const xloper12& op, detail::strict_tag tag) const
  {
    return static_cast<float>(as_double(op, tag));
  }
};

// std::string specialization
template <>
struct xloper12_converter<std::string> {
  // non-strict overload
  auto operator()(const xloper12& op) const
  {
    return as_string(op);
  }

  // strict overload
  auto operator()(const xloper12& op, detail::strict_tag tag) const
  {
    return as_string(op, tag);
  }
};

// std::wstring specialization
template <>
struct xloper12_converter<std::wstring> {
  // non-strict overload
  auto operator()(const xloper12& op) const
  {
    return as_wstring(op);
  }

  // strict overload
  auto operator()(const xloper12& op, detail::strict_tag tag) const
  {
    return as_wstring(op, tag);
  }
};

// std::wstring_view specialization
template <>
struct xloper12_converter<std::wstring_view> {
  auto operator()(const xloper12& op) const
  {
    return as_wstring_view(op);
  }
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

// integral type partial specialization
template <std::integral T>
struct xloper12_converter<T> {
  // non-strict overload
  auto operator()(const xloper12& op) const
  {
    if constexpr (std::same_as<std::remove_cv_t<T>, bool>)
      return as_bool(op);
    else
      return static_cast<T>(as_int(op));
  }

  // strict overload
  auto operator()(const xloper12& op, detail::strict_tag tag) const
  {
    if constexpr (std::same_as<std::remove_cv_t<T>, bool>)
      return as_bool(op, tag);
    else
      return static_cast<T>(as_int(op, tag));
  }
};

namespace detail {

/**
 * Defaulter that is a no-op.
 *
 * This indicates that no default is available.
 */
struct null_defaulter { constexpr void operator()() const noexcept {} };

/**
 * Concept for a valid defaulter.
 *
 * This must either convert to a specified type or return `void`.
 *
 * @tparam F Nullary invocable
 * @tparam T Target type
 */
template <typename F, typename T>
concept arg_spec_defaulter =
  requires (F f) { { std::invoke(f) } -> std::convertible_to<T>; } ||
  requires (F f) { { std::invoke(f) } -> std::same_as<void>; };

/**
 * Concept for a valid strict conversion indicator.
 *
 * This must either be `strict_tag` or `void`.
 *
 * @tparam T type
 */
template <typename T>
concept maybe_strict = std::same_as<T, strict_tag> || std::same_as<T, void>;

}  // namespace detail

/**
 * `xloper12_converter<std::tuple<Ts...>>` argument specification.
 *
 * This serves as a light wrapper around a string argument name and a callable
 * used to provide an argument default if desired.
 *
 * @tparam F Nullary invocable returning `void` or a value type
 * @tparam S Strict conversion indicator
 */
template <std::invocable F, detail::maybe_strict S = void>
class arg_spec {
public:
  using value_type = std::invoke_result_t<F>;

  /**
   * Ctor.
   *
   * This uses the null defaulter to indicate that the parameter is required.
   *
   * @param name Parameter name
   */
  arg_spec(std::string_view name)
    : arg_spec{std::string_view{name}, detail::null_defaulter{}}
  {}

  /**
   * Ctor.
   *
   * This uses the null defaulter to indicate that the parameter is required
   * and also indicates that strict conversion should be specified.
   *
   * @param name Parameter name
   */
  arg_spec(std::string_view name, detail::strict_tag) : arg_spec{name} {}

  /**
   * Ctor.
   *
   * @note A defaulter that returns `void` can be used to indicate that the
   *  Excel parameter with name `name` is required.
   *
   * @param name Parameter name
   * @param defaulter Parameter value defaulter
   */
  arg_spec(std::string_view name, F defaulter)
    : name_{name}, defaulter_{std::move(defaulter)}
  {}

  /**
   * Ctor.
   *
   * This indicates that strict conversion should be specified.
   *
   * @param name Parameter name
   * @param defaulter Parameter value defaulter
   */
  arg_spec(std::string_view name, detail::strict_tag, F defaulter)
    : arg_spec{name, std::move(defaulter)}
  {}

  /**
   * Return the argument name.
   */
  auto& name() const noexcept { return name_; }

  /**
   * Return the default value to use if no value is provided from Excel.
   */
  auto operator()() const
  {
    return std::invoke(defaulter_);
  }

private:
  std::string_view name_;
  F defaulter_;
};

// user-defined deduction guide for single-parameter overload
arg_spec(std::string_view) -> arg_spec<detail::null_defaulter>;
arg_spec(std::string_view, detail::strict_tag) ->
  arg_spec<detail::null_defaulter, detail::strict_tag>;

/**
 * Partial specialization implementation for `std::tuple<Ts...>`.
 *
 * Unlike the other `xloper12_converter<T>` specializations we cannot use an
 * opaque function call and need to rely on templates more. This specialization
 * enables conversion of a table of Excel cells in a key-value format, e.g.
 *
 * @code
 * +-----+-------+
 * | Key | Value |
 * | ... |  ...  |
 * +-----+-------+
 * @endcode
 *
 * This format enables more flexible, position-independent, specification of
 * input parameters, while also allowing some to be omitted.
 *
 * @tparam Ts Target types
 */
template <typename... Ts>
struct xloper12_converter<std::tuple<Ts...>> {
private:
  /**
   * Traits to detect if a type is a `std::optional<T>`.
   *
   * @tparam T type
   */
  template <typename T>
  struct is_optional : std::false_type {};

  /**
   * Partial specialization for a `std::optional<T>`.
   *
   * @tparam T type
   */
  template <typename T>
  struct is_optional<std::optional<T>> : std::true_type {};

  /**
   * Convert key-value Excel rows into a tuple of C++ values.
   *
   * @tparam Is Indices 0 through `sizeof...(Ts)` - 1
   * @tparam Fs Nullary invocable value defaulters
   * @tparam Ss Strict conversion indicators
   *
   * @param op `XLOPER12` to convert
   * @param args Excel argument specifications
   */
  template <std::size_t... Is, std::invocable... Fs, detail::maybe_strict... Ss>
  auto operator()(
    std::index_sequence<Is...>,
    const xloper12& op,
    const arg_spec<Fs, Ss>&... args) const
  {
    // compile-time checks
    // note: sizeof...(Ss) and sizeof...(Fs) are guaranteed equal
    static_assert(sizeof...(Is) == sizeof...(Ts));
    static_assert(sizeof...(Ts) == sizeof...(Fs), "argument count mismatch");
    static_assert(
      (detail::arg_spec_defaulter<Fs, Ts> && ...),
      "not all argument defaults are valid (must convert to Ts or void)"
    );
    // get view since xloper12 is opaque
    oper12_view view{&op};
    // must be xltypeMulti
    if (view.type() != xltype::multi)
      throw std::runtime_error{
        std::string{"input must be xltypeMulti not "} +
        to_string(view.type())
      };
    // must have *two* columns
    if (view.cols() != 2)
      throw std::runtime_error{
        "xltypeMulti input must have 2 columns not " +
        std::to_string(view.cols())
      };
    // determine indices of all the arguments (Is -> arg row index)
    std::array<std::optional<std::size_t>, sizeof...(Is)> arg_map;
    for (auto i = 0u; i < view.rows(); i++) {
      // convert to argument name
      auto name_i = view(i, 0u).as<std::string>();
      // fold to filter out unknown names
      // note: need extra parentheses to tell compiler this is a fold
      if (((name_i != args.name()) && ...))
        throw std::runtime_error{
          "unknown named argument \"" + name_i + "\" not one of:" +
          (("\n    " + std::string{args.name()}) + ...)
        };
      // fold to check against each of the args (break early)
      (
        [i, &name_i, &args, &arg_map]
        {
          // no match so continue
          if (name_i != args.name())
            return true;
          // if there is a match but Is is already mapped to a value, error, as
          // it means that this named parameter was duplicated
          if (arg_map[Is])
            throw std::runtime_error{
              "named argument \"" + name_i + "\" duplicated at indices " +
              std::to_string(*arg_map[Is]) + " and " + std::to_string(i)
            };
          // otherwise we are mapping Is -> i to the first time so map + break
          arg_map[Is] = i;
          return false;
        }() && ...
      );
    }
    // convert all values
    return std::tuple{
      [&view, &args, &arg_map]
      {
        // mapping exists so an input was specified for the argument name
        if (arg_map[Is]) {
          // use strict conversion if specified
          if constexpr (std::same_as<Ss, detail::strict_tag>)
            return view(*arg_map[Is], 1u).as<Ts>(strict);
          else
            return view(*arg_map[Is], 1u).as<Ts>();
        }
        // if non-void defaulter provided, use that
        if constexpr (!std::is_void_v<decltype(args())>)
          return Ts{args()};
        // if a std::optional<T> construct an empty optional
        // note: you can have a std::optional<T> with a non-void defaulter
        // which is why we check the defaulter first
        else if constexpr (is_optional<Ts>::value)
          return Ts{};
        // otherwise throw as not std::optional<T> or with default value
        else
          throw std::runtime_error{
            "required argument " + std::string{args.name()} +
            " not provided and missing default"
          };
      }()...
    };
  }

public:
  /**
   * Convert key-value Excel rows into a tuple of C++ values.
   *
   * An exception is thrown if conversion cannot be done or if a required
   * argument is missing from the input. To specify that some arguments may not
   * have *any* argument at all, use a `std::optional<T>`.
   *
   * @tparam Is Indices 0 through `sizeof...(Ts)` - 1
   * @tparam Fs Nullary invocable value defaulters
   * @tparam Ss Strict conversion indicators
   *
   * @param op `XLOPER12` to convert
   * @param args Excel argument specifications
   */
  template <std::invocable... Fs, detail::maybe_strict... Ss>
  auto operator()(const xloper12& op, const arg_spec<Fs, Ss>&... args) const
  {
    return (*this)(std::index_sequence_for<Ts...>{}, op, args...);
  }
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_XL_CONV_H_
