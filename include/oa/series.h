/**
 * @file series.h
 * @author Derek Huang
 * @brief C++ header for a 1D series object supporting heterogenous data
 * @copyright MIT License
 */

#ifndef OA_SERIES_H_
#define OA_SERIES_H_

#include <algorithm>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <ranges>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#include "oa/rtti.h"
#include "oa/st_format.h"
#include "oa/warnings.h"

namespace oa {

/**
 * General object type for the `series` supporting all data types.
 *
 * `std::monostate` is used to represent a generic missing value.
 */
using series_object = std::variant<
  std::monostate,
  std::int32_t,
  std::int64_t,
  float,
  double,
  std::string
>;

/**
 * Scoped enum for the `series` data types.
 */
enum class series_type {
  int32,    // 32-bit int
  int64,    // 64-bit int
  float32,  // 32-bit float
  float64,  // 64-bit float
  string,   // std::string
  object    // series_object
};

/**
 * Traits type mapping a C++ type to `series_type`.
 *
 * @tparam T type
 */
template <typename T>
struct to_series_type_impl {};

/**
 * Traits type mapping `series_type` to a C++ type.
 *
 * @tparam type `series_type` value
 */
template <series_type type>
struct series_value_type {};

/**
 * Macro for `to_series_type_impl` and `series_value_type` specializations.
 *
 * @param cxx C++ type
 * @param mem `series_type` enumeration member
 */
#define OA_CXX_SERIES_TYPE_MAPPING(cxx, mem) \
  /* C++ type -> series_type */ \
  template <> \
  struct to_series_type_impl<cxx> { \
    static constexpr auto value = series_type::mem; \
  }; \
  /* series_type -> C++ type */ \
  template <> \
  struct series_value_type<series_type::mem> { \
    using type = cxx; \
  }

OA_CXX_SERIES_TYPE_MAPPING(std::int32_t, int32);
OA_CXX_SERIES_TYPE_MAPPING(std::int64_t, int64);
OA_CXX_SERIES_TYPE_MAPPING(float, float32);
OA_CXX_SERIES_TYPE_MAPPING(double, float64);
OA_CXX_SERIES_TYPE_MAPPING(std::string, string);
OA_CXX_SERIES_TYPE_MAPPING(series_object, object);

#undef OA_CXX_SERIES_TYPE_MAPPING

/**
 * Return the `series_type` value corresponding to the given type.
 *
 * @tparam T type
 */
template <typename T>
constexpr auto to_series_type() noexcept
{
  return to_series_type_impl<T>::value;
}

/**
 * SFINAE helper + type alias for the C++ type corresponding to `series_type`.
 *
 * @tparam type `series_type` value
 */
template <series_type type>
using series_value_type_t = typename series_value_type<type>::type;

/**
 * Return the string literal name associated with the `series_type`.
 *
 * @param v Series type
 */
constexpr auto to_string(series_type v) noexcept
{
#define OA_SERIES_TYPE_CASE(x) case series_type::x: return #x;
  switch (v) {
  OA_SERIES_TYPE_CASE(int32)
  OA_SERIES_TYPE_CASE(int64)
  OA_SERIES_TYPE_CASE(float32)
  OA_SERIES_TYPE_CASE(float64)
  OA_SERIES_TYPE_CASE(string)
  OA_SERIES_TYPE_CASE(object)
#undef OA_SERIES_TYPE_CASE
  default:
    return "(unknown)";
  }
}

/**
 * Write the `series_type` to the output stream.
 *
 * @param out Output stream
 * @param type Series type
 */
inline auto& operator<<(std::ostream& out, series_type type)
{
  return out << to_string(type);
}

/**
 * Traits providing the "closest" supported value type for the given type.
 *
 * Integral types are converted to `int32` or `int64` based on their size,
 * floating types are converted to `float32` or `float64` based on size, and
 * `std::string` convertible types are converted to `std::string`. Finally,
 * conversion to `series_object` is considered, accepting `std::monostate`.
 *
 * @note On LLP64 platforms like Windows `closest_series_type_t<long>` will be
 *  `std::int32_t`, which is unfortunately considered a narrowing conversion.
 *  `long` also cannot be correctly converted to `series_object` despite it
 *  being possible to convert `long` to `long long`. May be a VS 2022 bug.
 *
 * @tparam T Value type
 */
template <typename T>
struct closest_series_type {
  using type = std::conditional_t<
    // integral type
    std::is_integral_v<T>,
    std::conditional_t<
      // signed and fits in int32_t
      std::is_signed_v<T> && (sizeof(T) <= sizeof(std::int32_t)),
      std::int32_t,
      std::conditional_t<
        // signed and fits in int64_t
        std::is_signed_v<T> && (sizeof(T) <= sizeof(std::int64_t)),
        std::int64_t,
        // default to double
        // TODO: can separately support unsigned types
        double
      >
    >,
    std::conditional_t<
      // floating type
      std::is_floating_point_v<T>,
      // note: no support for extended-precision floating types
      std::conditional_t<sizeof(T) <= sizeof(float), float, double>,
      // non-floating type
      std::conditional_t<
        std::convertible_to<T, std::string>,
        std::string,
        // catch-all is series_object
        series_object
      >
    >
  >;
};

/**
 * SFINAE helper and type alias for the "closest" `series` value type.
 *
 * @tparam T Value type
 */
template <typename T>
using closest_series_type_t = typename closest_series_type<T>::type;

/**
 * 1D series data type supporting heterogenous storage.
 *
 * If possible, the series is of a single data type, e.g. `float64`, which
 * provides the best performance, as native C++ buffer access is possible.
 * Otherwise, the series contains a buffer of `series_object` variants.
 */
class series {
private:
  /**
   * Helper template to generate `storage_type`.
   *
   * @tparam Ts Storage element types
   */
  template <typename... Ts>
  struct make_storage_type {
    using type = std::variant<std::vector<Ts>...>;
  };

  /**
   * Helper type alias for `storage_type`.
   *
   * @tparam Ts types
   */
  template <typename... Ts>
  using make_storage_type_t = typename make_storage_type<Ts...>::type;

  /**
   * Visitor type for retrieving the series type.
   */
  class type_visitor {
  public:
    /**
     * Return the corresponding enumeration value for the series type.
     *
     * @tparam T Column storage type
     */
    template <typename T>
    constexpr auto operator()(const T& /*data*/) const noexcept
    {
      return to_series_type<std::ranges::range_value_t<T>>();
    }
  };

  /**
   * Visitor type for retrieving the series size.
   */
  class size_visitor {
  public:
    /**
     * Return the number of elements in the series.
     *
     * @tparam T Column storage type
     *
     * @param data Column storage
     */
    template <typename T>
    auto operator()(const T& data) const
    {
      return std::ranges::size(data);
    }
  };

  /**
   * Visitor type for checking the C++ series data type.
   *
   * We use this instead of `std::holds_alternative` because the latter is
   * ill-formed if passed a type not supported by the given variant.
   *
   * @tparam T Value type
   */
  template <typename T>
  class type_check_visitor {
  public:
    /**
     * Indicate if the series storage value type is the specified type.
     *
     * @tparam U Column storage type
     */
    template <typename U>
    constexpr bool operator()(const U& /*data*/) const noexcept
    {
      return std::is_same_v<T, std::ranges::range_value_t<U>>;
    }
  };

  /**
   * Visitor type for getting the address of the specified element.
   *
   * The value type is typed to `void` by default in which case no additional
   * checking for type aliasing is needed. However, if a non-`void` type is
   * specified, an exception is thrown if type aliasing would occur.
   *
   * @tparam T Value type
   */
  template <typename T = void>
  class address_visitor {
  private:
    /**
     * Add const to `T` based on another type's const-qualification.
     *
     * @tparam U type
     */
    template <typename U>
    using maybe_const_t = std::conditional_t<std::is_const_v<U>, const T, T>;

    /**
     * Check if a range's value type matches `T` or indicate if `T` is `void`.
     *
     * In either case we can convert to a pointer type appropriately.
     *
     * @tparam R Range type
     */
    template <typename R>
    static constexpr bool is_aliasable = (
      std::is_void_v<T> ||
      std::is_same_v<T, std::ranges::range_value_t<R>>
    );

  public:
    /**
     * Ctor.
     *
     * @param i Element index
     */
    address_visitor(std::size_t i) noexcept : i_{i} {}

    /**
     * Return the address of the specified element in the storage.
     *
     * Appropriate const-qualification is added based on the reference type. If
     * the `address_visitor` type parameter does not match the value type of
     * the storage but is not `void` an exception will be thrown.
     *
     * @tparam U Column storage type
     *
     * @param data Column storage
     */
    template <typename U>
    maybe_const_t<U>* operator()(U& data) const noexcept(is_aliasable<U>)
    {
      if constexpr (is_aliasable<U>)
        return &data[i_];
      else
        throw std::runtime_error{
          "cannot address " +
          std::string{type_name(typeid(std::ranges::range_value_t<U>))} +
          " buffer as " + std::string{type_name(typeid(T))}
        };
    }

  private:
    std::size_t i_;
  };

  /**
   * Visitor type for converting a `series_object` to the specified C++ type.
   *
   * @tparam T type
   */
  template <typename T>
  class object_visitor {
  public:
    /**
     * Return the contained value converted to the target type if possible.
     *
     * If conversion is not possible an exception is simply thrown.
     *
     * @note Return type fixed instead of `auto` due to `constexpr` branch.
     *
     * @tparam U `series_object` value type
     */
    template <typename U>
    T operator()(const U& v) const
    {
      // if convertible we can return the value
      // note: static_cast for avoiding some warnings about narrowing
      if constexpr (std::convertible_to<U, T>)
        return static_cast<T>(v);
      // note: can't do anything with std::monostate if T is not series_object
      else if constexpr (std::is_same_v<U, std::monostate>)
        throw std::runtime_error{
          "cannot convert std::monostate to target type " +
          std::string{type_name(typeid(T))}
        };
      // can't do anything with non-convertible types
      // note: can use to_series_type<U>() to support remaining types
      else
        throw std::runtime_error{
          "cannot convert " + std::string{type_name(typeid(U))} +
          " to target type " + std::string{type_name(typeid(T))}
        };
    }
  };

  /**
   * Visitor type for equality comparing `series_object` against a C++ value.
   *
   * @tparam T type
   */
  template <typename T>
  class object_equals {
  public:
    /**
     * Ctor.
     *
     * @param v Value to compare against
     */
    object_equals(const T& v) noexcept : v_{v} {}

    /**
     * Indicate if the contained `series_object` equals the given value.
     *
     * If the types are not equality-comparable `false` is returned.
     *
     * @tparam U `series_object` value type
     */
    template <typename U>
    bool operator()(const U& v) const
    {
      if constexpr (std::equality_comparable_with<T, U>)
        return v == v_;
      else
        return false;
    }

  private:
    const T& v_;
  };

  /**
   * Visitor type for writing the `series_object` to `std::ostream`.
   */
  class object_stream_formatter {
  public:
    /**
     * Ctor.
     *
     * @param out Output stream
     */
    object_stream_formatter(std::ostream& out) noexcept : out_{&out} {}

    /**
     * Write the `series_object` to the output stream.
     *
     * If `std::monostate` is contained `"NA" is written to the stream.
     *
     * @tparam U `series_object` value type
     */
    template <typename T>
    auto& operator()(const T& v) const
    {
      if constexpr (std::is_same_v<T, std::monostate>)
        return *out_ << "NA";
      else
        return *out_ << v;
    }

  private:
    std::ostream* out_;
  };

public:
  /**
   * Storage type.
   *
   * Column storage is a vector of any of the `series_type` types.
   *
   * @note Default-constructed storage type is of `series_object`.
   */
  using storage_type = make_storage_type_t<
    series_object,
    std::int32_t,
    std::int64_t,
    float,
    double,
    std::string
  >;

  /**
   * Default ctor.
   *
   * This is provided for convenience but results in an invalid `series`.
   */
  series() noexcept = default;

private:
  /**
   * Indicates if a range is convertible to `storage_type`.
   *
   * @tparam R Range type
   */
  template <typename R>
  static constexpr bool is_convertible_range =
    std::convertible_to<R, storage_type>;

  /**
   * Indicates if a range's elements are convertible to `series_object`.
   *
   * @tparam R Range type
   */
  template <typename R>
  static constexpr bool is_value_convertible_range =
    std::convertible_to<std::ranges::range_value_t<R>, series_object>;

  /**
   * Visitor type for copying values to new, compatible storage.
   *
   * This is an essential operation for re-allocating the `series` storage when
   * type widening is required, e.g. setting a `std::string` on `float` data.
   * Although this obviously a large performance hit, we still allow this
   * operation to provide increased usage flexibility.
   *
   * @tparam T Target value type
   */
  template <typename T>
  class realloc_visitor {
  public:
    using storage_type = std::vector<T>;

    /**
     * Return new, compatible storage from the `series` storage.
     *
     * Values are copied and converted as necessary. An exception is thrown if
     * there is no possible conversion to the target value type.
     *
     * @tparam U Column storage type
     *
     * @param data Column storage
     */
    template <typename U>
    storage_type operator()(const U& data) const
    {
      // if convertible
      if constexpr (std::convertible_to<std::ranges::range_value_t<U>, T>) {
        storage_type out(std::ranges::size(data));
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4244)
        std::ranges::copy(data, std::ranges::begin(out));
OA_MSVC_WARNING_POP()
        // note: direct-init since C++17 so not a copy
        return out;
      }
      // not convertible
      // note: necessary since operator() must compile for all alternatives
      else
        throw std::runtime_error{
          "cannot reallocate " +
          std::string{type_name(typeid(std::ranges::range_value_t<U>))} +
          " storage as " + std::string{type_name(typeid(T))} + " storage"
        };
    }
  };

public:
  /**
   * Ctor.
   *
   * Constructs from a range convertible to the storage type. Transfer of
   * ownership from a compatible `std::vector` can be done via move.
   *
   * @note Trailing `std::monostate` is for overload disambiguation.
   *
   * @tparam R Forward range convertible to `storage_type`
   *
   * @param data Series data range
   */
  template <std::ranges::forward_range R>
  requires (is_convertible_range<R>)
  series(R&& data, std::monostate = {}) : data_{std::forward<R>(data)} {}

  /**
   * Ctor.
   *
   * Constructs from a range convertible to the storage type. Transfer of
   * ownership from a compatible `std::vector` can be done via move.
   *
   * This overload allows for the `series` to have a name.
   *
   * @note Trailing `std::monostate` is for overload disambiguation.
   *
   * @tparam R Forward range convertible to `storage_type`
   *
   * @param name Series name
   * @param data Series data range
   */
  template <std::ranges::forward_range R>
  requires (is_convertible_range<R>)
  series(std::string name, R&& data, std::monostate = {})
    : series{std::forward<R>(data), std::monostate{}}
  {
    name_ = std::move(name);
  }

  /**
   * Ctor.
   *
   * Constructs from a range not directly convertible to the storage type but
   * which has elements that are convertible to `series_object`. This ctor will
   * necessarily need to perform converting copies but will attempt to use the
   * corresponding range value type if possible instead of `series_object`.
   *
   * @note Trailing `std::monostate` is for overload disambiguation.
   *
   * @tparam R Forward range with elements convertible to `series_object`
   *
   * @param data Series data range
   */
  template <std::ranges::forward_range R>
  requires (!is_convertible_range<R> && is_value_convertible_range<R>)
  series(R&& data, std::monostate = {})
  {
    // get closest storage value type from R's value type
    using data_type = closest_series_type_t<std::ranges::range_value_t<R>>;
    // copy values to new storage vector + move-assign
    std::vector<data_type> res(std::ranges::size(data));
    std::ranges::copy(data, res.begin());
    data_ = std::move(res);
  }

  /**
   * Ctor.
   *
   * Constructs from a range not directly convertible to the storage type but
   * which has elements that are convertible to `series_object`. This ctor will
   * necessarily need to perform converting copies but will attempt to use the
   * corresponding range value type if possible instead of `series_object`.
   *
   * This overload allows for the `series` to have a name.
   *
   * @note Trailing `std::monostate` is for overload disambiguation.
   *
   * @tparam R Forward range with elements convertible to `series_object`
   *
   * @param name Series name
   * @param data Series data range
   */
  template <std::ranges::forward_range R>
  requires (!is_convertible_range<R> && is_value_convertible_range<R>)
  series(std::string name, R&& data, std::monostate = {})
    : series{std::forward<R>(data), std::monostate{}}
  {
    name_ = std::move(name);
  }

  /**
   * Ctor.
   *
   * Constructs from an initializer list of `series_object` convertible values.
   * The values will be copied to the storage type "closest" to `T`.
   *
   * @tparam T Value type
   *
   * @param data Series data range
   */
  template <std::convertible_to<series_object> T>
  series(std::initializer_list<T> data) : series{data, std::monostate{}} {}

  /**
   * Ctor.
   *
   * Constructs from an initializer list of `series_object` convertible values.
   * The values will be copied to the storage type "closest" to `T`.
   *
   * This overload allows for the `series` to have a name.
   *
   * @tparam T Value type
   *
   * @param name Series name
   * @param data Series data range
   */
  template <std::convertible_to<series_object> T>
  series(std::string name, std::initializer_list<T> data)
    : series{std::move(name), data, std::monostate{}}
  {}

  /**
   * Ctor.
   *
   * Constructs from an initializer list of `series_object` values. This
   * overload enables list-init with values of different types.
   *
   * @param data Series data range
   */
  series(std::initializer_list<series_object> data)
    : series{data, std::monostate{}}
  {}

  /**
   * Ctor.
   *
   * Constructs from an initializer list of `series_object` values and a name.
   *
   * @param name Series name
   * @param data Series data range
   */
  series(std::string name, std::initializer_list<series_object> data)
    : series{std::move(name), data, std::monostate{}}
  {}

  // TODO: add ctors for iterator pair

  /**
   * Return reference to the series' name.
   */
  auto& name() noexcept { return name_; }

  /**
   * Return const reference to the series' name.
   */
  auto& name() const noexcept { return name_; }

  /**
   * Return the series value type.
   */
  auto type() const
  {
    return std::visit(type_visitor{}, data_);
  }

  /**
   * Indicate if the series storage is of the given C++ type.
   *
   * @tparam T Column value type
   */
  template <typename T>
  bool type() const
  {
    return std::visit(type_check_visitor<T>{}, data_);
  }

  /**
   * Return the number of elements in the series.
   */
  auto size() const
  {
    return std::visit(size_visitor{}, data_);
  }

  /**
   * Return a statically typed view over the series data.
   *
   * The specified C++ value type must match that of the storage. If not, an
   * exception is thrown to indicate the type mismatch.
   *
   * @tparam T View value type
   */
  template <typename T>
  std::span<T> view()
  {
    return {std::visit(address_visitor<T>{0}, data_), size()};
  }

  /**
   * Return a statically typed read-only view over the series data.
   *
   * The specified C++ value type must match that of the storage. If not, an
   * exception is thrown to indicate the type mismatch.
   *
   * @tparam T View value type
   */
  template <typename T>
  std::span<const T> view() const
  {
    return {std::visit(address_visitor<T>{0}, data_), size()};
  }

  /**
   * Return a reference to the specified element in te series data.
   *
   * The specified C++ value type must match that of the storage. If not, an
   * exception is thrown to indicate the type mismatch.
   *
   * @note It is more performant to call `view<T>()` and then manipulate the
   *  returned `std::span<T>` instead of repeatedly calling `at<T>()`.
   *
   * @tparam T Value type
   *
   * @param i Element index
   */
  template <typename T>
  auto& at(std::size_t i)
  {
    return view<T>()[i];
  }

  /**
   * Return a const reference to the specified element in the series data.
   *
   * The specified C++ value type must match that of the storage. If not, an
   * exception is thrown to indicate the type mismatch.
   *
   * @note It is more performant to call `view<T>()` and then manipulate the
   *  returned `std::span<const T>` instead of repeatedly calling `at<T>()`.
   *
   * @tparam T Value type
   *
   * @param i Element index
   */
  template <typename T>
  auto& at(std::size_t i) const
  {
    return view<T>()[i];
  }

  /**
   * Value proxy template type.
   *
   * Since the backing storage of the series is of variable type itself we need
   * a proxy object for a particular desired series element. This proxy object
   * references the ownning series and the index of the series element and
   * provides implicit conversions to C++ types for natural manipulation.
   *
   * Modification of the underlying value is allowed for `value_proxy` with
   * non-const `series` template type. In this case, the available `operator=`
   * will first attempt direct conversion to the storage type, but if not
   * possible, will re-allocate new storage of a wider type.
   *
   * @tparam S `series` or `const series`
   */
  template <typename S>
  requires (std::is_same_v<std::remove_const_t<S>, series>)
  class value_proxy {
  private:
    // note: series::operator[] and series::iterator::operator* need access
    friend class series;

    /**
     * Ctor.
     *
     * Creates a new `value_proxy` representing the `i`th value in the series.
     *
     * @param s Parent series
     * @param i Index of desired value
     */
    value_proxy(S& s, std::size_t i) noexcept : s_{&s}, i_{i} {}

    /**
     * Deleted copy ctor.
     */
    value_proxy(const value_proxy&) = delete;

    /**
     * Return the address of the proxied series value.
     *
     * The pointer is `void*` or `const void*` as appropriate.
     */
    auto address() const
    {
      return std::visit(address_visitor<>{i_}, s_->data_);
    }

    /**
     * Return a typed reference to the proxied series value.
     *
     * The reference is automatically const-qualified as appropriate.
     *
     * @tparam T type
     */
    template <typename T>
    auto& value() const
    {
      return *std::visit(address_visitor<T>{i_}, s_->data_);
    }

    /**
     * Return the type-converted value in the proxied `series_object`.
     *
     * Assuming the proxied value is a `series_object`, this member function
     * returns the value contained in the `series_object`, but converted to the
     * target type. Therefore this function returns a value, not a reference.
     *
     * If the proxied value is not a `series_object` an exception is thrown.
     *
     * @tparam T type
     */
    template <typename T>
    auto object_value() const
    {
      return std::visit(object_visitor<T>{}, value<series_object>());
    }

  public:
    /**
     * Return the type of the series owning the proxied value.
     */
    auto type() const
    {
      return s_->type();
    }

  private:
    /**
     * Implement a forcible conversion to arithmetic type.
     *
     * If the proxied value cannot be converted an exception is thrown. Using
     * this function subjects the caller to truncation error depending on the
     * target type and thus must be used with discretion, e.g. to prevent sign
     * loss when converting from signed/unsigned, etc.
     *
     * @tparam T Arithmetic type
     */
    template <typename T>
    requires (std::is_arithmetic_v<T>)
    auto as() const
    {
      switch (type()) {
      case series_type::int32:
        return static_cast<T>(value<std::int32_t>());
      case series_type::int64:
        return static_cast<T>(value<std::int64_t>());
      case series_type::float32:
        return static_cast<T>(value<float>());
      case series_type::float64:
        return static_cast<T>(value<double>());
      case series_type::object:
        return object_value<T>();
      default:
        throw std::runtime_error{
          "cannot convert " + std::string{to_string(type())} + " to " +
          std::string{type_name(typeid(T))}
        };
      }
    }

  public:
    /**
     * Implicitly convert to `int`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator int() const
    {
      return as<int>();
    }

    /**
     * Implicitly convert to `long`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator long() const
    {
      return as<long>();
    }

    /**
     * Implicitly convert to `long long`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator long long() const
    {
      return as<long long>();
    }

    /**
     * Implicitly convert to a `float`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator float() const
    {
      return as<float>();
    }

    /**
     * Implicitly convert to a `double`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator double() const
    {
      return as<double>();
    }

    // TODO: add unsigned conversions + suppress resulting compiler warnings

    /**
     * Implicitly convert to a `std::string`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     *
     * @note Arithmetic types are converted using `std::to_string()`.
     */
    operator std::string() const
    {
      switch (type()) {
      case series_type::int32:
        return std::to_string(value<std::int32_t>());
      case series_type::int64:
        return std::to_string(value<std::int64_t>());
      case series_type::float32:
        return std::to_string(value<float>());
      case series_type::float64:
        return std::to_string(value<double>());
      case series_type::string:
        return value<std::string>();
      case series_type::object:
        return object_value<std::string>();
      default:
        throw std::runtime_error{
          "cannot convert " + std::string{to_string(type())} + " to std::string"
        };
      }
    }

    // TODO: add operator std::string_view()

private:
    /**
     * Helper traits to check if the common type of `T` and `U` is `T`.
     *
     * @tparam T Target type
     * @tparam U Candidate type
     */
    template <typename T, typename U, typename = void>
    struct is_common_with_first : std::false_type {};

    /**
     * Partial specialization when `std::common_type_t<T, U>` is `T`.
     *
     * @tparam T Target type
     * @tparam U Candidate type
     */
    template <typename T, typename U>
    struct is_common_with_first<
      T, U, std::enable_if_t<std::is_same_v<T, std::common_type_t<T, U>>> >
      : std::true_type {};

    /**
     * Indicate if the common type of `T` and `U` is `T`.
     *
     * @tparam T Target type
     * @tparam U Candidate type
     */
    template <typename T, typename U>
    static constexpr bool is_common_with_first_v =
      is_common_with_first<T, U>::value;

    /**
     * Value assignment and re-allocation helper.
     *
     * If the incoming type matches or can be promoted to the current series
     * value type, forwarding assignment, made explicit for `std::string`, is
     * performed. Otherwise, storage is re-allocated, existing elements are
     * promoted to the closest series type to `T`, and then assignment is done.
     *
     * @tparam type `series_type` value
     * @tparam T type
     *
     * @param v Value to assign
     */
    template <series_type type, typename T>
    auto& assign(T&& v)
    {
      // current series value type
      using V = series_value_type_t<type>;
      // get "closest" series type for T
      using U = closest_series_type_t<std::decay_t<T>>;
      // required series value type to accommodate U
      using RV = std::conditional_t<
        // if U can be promoted to V, use V
        is_common_with_first_v<V, U>,
        V,
        // if V can be promoted to U, use U, otherwise series_object
        std::conditional_t<is_common_with_first_v<U, V>, U, series_object>
      >;
      // required value type is same as current
      if constexpr (std::is_same_v<V, RV>)
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4244)
        value<V>() = std::forward<T>(v);
OA_MSVC_WARNING_POP()
      // required value type requires promotion/conversion
      else {
        // re-allocate + modify storage
        auto new_data = std::visit(realloc_visitor<RV>{}, s_->data_);
        new_data[i_] = RV{std::forward<T>(v)};
        s_->data_ = std::move(new_data);
      }
      // done
      return *this;
    }

public:
    /**
     * Assign a new value through the value proxy.
     *
     * This member is enabled when the owning `series` is non-const. If the new
     * value is the same or can be converted to the current value type, the
     * corresponding element is assigned. Otherwise, if current value type is
     * convertible to the closest series type for decayed `T`, storage is
     * re-allocated, values are copied, and the new value is assigned.
     *
     * @note Member is rvalue reference qualified to disable value modification
     *  through a direct-initialized instance of the `value_proxy`.
     *
     * @tparam T type
     *
     * @param v Value to assign
     */
    template <typename T>
    requires (!std::is_const_v<S>)
    auto& operator=(T&& v) &&
    {
      // if type matches or can be promoted
      switch (type()) {
      case series_type::int32:
        return assign<series_type::int32>(std::forward<T>(v));
      case series_type::int64:
        return assign<series_type::int64>(std::forward<T>(v));
      case series_type::float32:
        return assign<series_type::float32>(std::forward<T>(v));
      case series_type::float64:
        return assign<series_type::float64>(std::forward<T>(v));
      case series_type::string:
        return assign<series_type::string>(std::forward<T>(v));
      case series_type::object:
        return assign<series_type::object>(std::forward<T>(v));
      default:
        throw std::runtime_error{
          "cannot assign value to index " + std::to_string(i_) +
          " for series of type " + to_string(type())
        };
      }
    }

    /**
     * Compare against a value for equality.
     *
     * Actual equality comparison is delegated to the `operator==` of the
     * corresponding C++ type, and if the proxied value is not of said type,
     * then `false` is returned to indicate inequality.
     *
     * @todo Reimplement as `operator<=>` returning `std::partial_ordering` and
     *  `operator==` can be implemented via `operator<=>`.
     *
     * @tparam T type
     *
     * @param other Value to compare against
     */
    template <typename T>
    bool operator==(T&& other) const
    {
      switch (type()) {
      case series_type::int32:
        if constexpr (std::equality_comparable_with<T, std::int32_t>)
          return value<std::int32_t>() == other;
        break;
      case series_type::int64:
        if constexpr (std::equality_comparable_with<T, std::int64_t>)
          return value<std::int64_t>() == other;
        break;
      case series_type::float32:
        if constexpr (std::equality_comparable_with<T, float>)
          return value<float>() == other;
        break;
      case series_type::float64:
        if constexpr (std::equality_comparable_with<T, double>)
          return value<double>() == other;
        break;
      case series_type::string:
        if constexpr (std::equality_comparable_with<T, std::string>)
          return value<std::string>() == other;
        break;
      case series_type::object:
        return std::visit(object_equals{other}, value<series_object>());
      }
      // not comparable
      return false;
    }

    /**
     * Write the `value_proxy` to the output stream.
     *
     * This uses the `operator<<` associated with the contained value type and
     * streams `NA` if proxying an object type with missing value.
     *
     * @param out Output stream
     */
    auto& operator>>(std::ostream& out) const
    {
      switch (type()) {
      case series_type::int32:
        return out << value<std::int32_t>();
      case series_type::int64:
        return out << value<std::int64_t>();
      case series_type::float32:
        return out << value<float>();
      case series_type::float64:
        return out << value<double>();
      case series_type::string:
        return out << value<std::string>();
      default:
        return std::visit(object_stream_formatter{out}, value<series_object>());
      }
    }

    /**
     * Write the `value_proxy` to the output stream using its `operator>>`.
     *
     * @param out Output stream
     * @param vp `series` value proxy
     */
    friend auto& operator<<(std::ostream& out, const value_proxy& vp)
    {
      return vp >> out;
    }

  private:
    S* s_;           // series owning value
    std::size_t i_;  // index of proxied value
  };

  /**
   * Return a write-enabled proxy object to the specified series value.
   *
   * @param i Column element index
   */
  auto operator[](std::size_t i) noexcept
  {
    return value_proxy{*this, i};
  }

  /**
   * Return a write-disabled proxy object to the specified series value.
   *
   * @param i Column element index
   */
  auto operator[](std::size_t i) const noexcept
  {
    return value_proxy{*this, i};
  }

  /**
   * Iterator template type.
   *
   * This satisfies the `std::random_access_iterator` concept and when
   * dereferenced returnns the corresponding `value_proxy`. The `value_proxy`
   * can be modified if its `series` type is const-qualified.
   *
   * @tparam S `series` or `const series`
   */
  template <typename S>
  requires (std::is_same_v<std::remove_const_t<S>, series>)
  class iterator {
  public:
    // type aliases to satisfy iterator traits
    using difference_type = std::ptrdiff_t;
    using value_type = value_proxy<S>;

    /**
     * Default ctor.
     *
     * This satisfies `std::semiregular` and produces an invalid iterator.
     */
    iterator() noexcept : s_{}, i_{} {}

    /**
     * Ctor.
     *
     * This returns an iterator to the first element in the series.
     *
     * @param s Parent series
     */
    iterator(S& s) noexcept : iterator{s, 0u} {}

    /**
     * Ctor.
     *
     * @param s Parent series
     * @param i Element index in series
     */
    iterator(S& s, std::size_t i) noexcept : s_{&s}, i_{i} {}

    /**
     * Return the `value_proxy` associated with the iterator.
     *
     * This satisfies `std::indirectly_readable`.
     */
    auto operator*() const noexcept
    {
      return value_proxy{*s_, i_};
    }

    /**
     * Pre-increment operator.
     *
     * This satisfies `std::weakly_incrementable`.
     */
    auto& operator++() noexcept
    {
      i_++;
      return *this;
    }

    /**
     * Post-increment operator.
     *
     * This satisfies `std::incrementable`.
     */
    auto operator++(int) noexcept
    {
      auto prev = *this;
      ++(*this);
      return prev;
    }

    /**
     * Compare against another `iterator` for equality.
     *
     * This satisfies `std::equality_comparable` for `std::sentinel_for`.
     *
     * @param other Iterator to compare against
     */
    bool operator==(const iterator& other) const noexcept
    {
      return (*this <=> other) == 0;
    }

    /**
     * Pre-decrement operator.
     *
     * This satisfies `std::bidirectional_iterator`.
     */
    auto& operator--() noexcept
    {
      i_--;
      return *this;
    }

    /**
     * Post-decrement operator.
     *
     * This satisfies `std::bidirectional_iterator`.
     */
    auto operator--(int) noexcept
    {
      auto prev = *this;
      --(*this);
      return prev;
    }

    /**
     * Increment the iterator.
     *
     * This satisfies `std::random_access_iterator`.
     *
     * @param n Number of increments
     */
    auto& operator+=(difference_type n) noexcept
    {
      i_ += n;
      return *this;
    }

    /**
     * Decrement the iterator.
     *
     * This satisfies `std::random_access_iterator`.
     *
     * @param n Number of decrements
     */
    auto& operator-=(difference_type n) noexcept
    {
      i_ -= n;
      return *this;
    }

    /**
     * Return a new incremented iterator.
     *
     * This satisfies `std::random_access_iterator`.
     *
     * @param n Number of increments
     */
    auto operator+(difference_type n) const noexcept
    {
      return iterator{*s_, i_ + n};
    }

    /**
     * Return a new incremented iterator.
     *
     * This satisfies `std::random_access_iterator.
     *
     * @note This non-member function need not be a friend but in-class
     *  definition is more convenient than out-of-class definition.
     *
     * @param n Number of increments
     * @param it Starting iterator
     */
    friend auto operator+(difference_type n, const iterator& it) noexcept
    {
      return it + n;
    }

    /**
     * Return a new decremented iterator.
     *
     * This satisfies `std::random_access_iterator`.
     *
     * @param n Number of decrements
     */
    auto operator-(difference_type n) const noexcept
    {
      return iterator{*s_, i_ - n};
    }

    /**
     * Return the `value_proxy` associated with an incremented iterator.
     *
     * This satisfies `std::random_access_iterator`.
     *
     * @param n Number of increments
     */
    auto operator[](difference_type n) const noexcept
    {
      return *(*this + n);
    }

    /**
     * Return the number of increments between this and the other iterator.
     *
     * This satisfies `std::sized_sentinel_for`.
     *
     * @param other Iterator to count increments against
     */
    auto operator-(const iterator& other) const noexcept
    {
      // note: behavior unspecified if other is not reachable
      // if i_ larger just cast difference
      if (i_ > other.i_)
        return static_cast<difference_type>(i_ - other.i_);
      // otherwise, negate casted reversed difference
      else
        return -static_cast<difference_type>(other.i_ - i_);
    }

    /**
     * Three-way comparison operator.
     *
     * This satisfies `std::totally_ordered` for `std::random_access_iterator`.
     *
     * @param other Iterator to compare against
     */
    std::partial_ordering operator<=>(const iterator& other) const noexcept
    {
      // if other is not reachable then no ordering
      if (s_ != other.s_)
        return std::partial_ordering::unordered;
      // otherwise compare offset
      return i_ <=> other.i_;
    }

  private:
    S* s_;           // series owning value
    std::size_t i_;  // index of target value
  };

  /**
   * Return an iterator to the first `series` element.
   */
  auto begin() noexcept
  {
    return iterator{*this};
  }

  /**
   * Return a const iterator to the first `series` element.
   */
  auto begin() const noexcept
  {
    return iterator{*this};
  }

  /**
   * Return an iterator to one past the last `series` element.
   */
  auto end() noexcept
  {
    return iterator{*this, size()};
  }

  /**
   * Return a const iterator to one past the last `series` element.
   */
  auto end() const noexcept
  {
    return iterator{*this, size()};
  }

private:
  std::string name_;
  storage_type data_;
};

/**
 * Return a string representation for the `value_proxy`.
 *
 * This uses the `operator<<` overload for consistent formatting.
 *
 * @tparam T `series` or `const series`
 *
 * @param vp `series` value proxy
 */
template <typename T>
requires (std::is_same_v<std::remove_const_t<T>, series>)
auto to_string(const series::value_proxy<T>& vp)
{
  std::stringstream ss;
  ss << vp;
  // note: since C++20 using the rvalue ref-qualified overload enables move
  return std::move(ss).str();
}

/**
 * Write the `series` to the table formatter's output stream.
 *
 * The `series` is formatted similar to a `pandas.Series` with a `RangeIndex`.
 * For example, `oa::series{1, 2, 3}` would be printed as:
 *
 * @code
 * 0    1
 * 1    2
 * 2    3
 * dtype: int32
 * @endcode
 *
 * The named series `oa::series{"test", {2., 4., 6.}}` would be printed as:
 *
 * @code
 * 0    2
 * 1    4
 * 2    6
 * Name: test, dtype: float64
 * @endcode
 *
 * Under default formatting settings a truncated view is printed. For example,
 * given a `float` range `r` with 10000 elements, `oa::series{"big_test", r}`
 * would have only the first 5 and last 5 lines printed, i.e. as:
 *
 * @code
 * 0          0
 * 1          1
 * 2          2
 * 3          3
 *         ...
 * 9995    9995
 * 9996    9996
 * 9997    9997
 * 9998    9998
 * 9999    9999
 * Name: big_test, Length: 10000, dtype: float32
 * @endcode
 *
 * @param fmt Stream formatter
 * @param ser Series to write
 */
inline auto& operator<<(const st_formatter& fmt, const series& ser)
{
  // output stream + formatting options
  auto& out = fmt.stream();
  auto& opts = fmt.options();
  // indicate if truncated representation is printed
  bool truncate = opts.excess_rows(ser.size());
  // print width of index column + string representations
  // note: truncated size has 5 begin, 5 end rows, with ... separating
  std::size_t index_width = 0u;
  std::vector<std::string> index_strs(truncate ? 11u : ser.size());
  // print width of index values + string representations
  // note: if truncating, "... " is printed as a value, which has width 4
  std::size_t col_width = (truncate) ? 4u : 0u;
  std::vector<std::string> col_strs(truncate ? 11u : ser.size());
  // first ending and second starting offsets for index/values
  std::size_t i1 = truncate ? 5u : ser.size();
  std::size_t i2 = truncate ? ser.size() - 5u : ser.size();
  // determine widths + compute string representations
  // first loop
  for (std::size_t i = 0u; i < i1; i++) {
    // compute string representations
    index_strs[i] = std::to_string(i);
    // note: cannot implicitly convert due to std::string::operator=. this also
    // would give us a slightly different result for arithmetic values
    col_strs[i] = to_string(ser[i]);
    // update index + col widths
    index_width = (std::max)(index_width, index_strs[i].size());
    col_width = (std::max)(col_width, col_strs[i].size());
  }
  // second loop (truncation only)
  // note: write from i1 + 1 since i1 is used for the blank space
  for (std::size_t i = i2; i < ser.size(); i++) {
    // offset into index_strs, col_strs
    auto j = i - i2 + i1 + 1;
    // compute string representations
    index_strs[j] = std::to_string(i);
    col_strs[j] = to_string(ser[i]);
    // update indes + col widths
    index_width = (std::max)(index_width, index_strs[j].size());
    col_width = (std::max)(col_width, col_strs[j].size());
  }
  // if truncating, fill in index i1 with the appropriate filler value
  // note: index_strs[i1] is already default-constructed
  if (truncate)
    col_strs[i1] = "... ";
  // write values to stream with padding
  // note: use the lengths of index_strs and col_strs
  for (std::size_t i = 0u; i < index_strs.size(); i++) {
    out <<
      // index + padding + separating 4 spaces
      index_strs[i] << padding{4u + index_width - index_strs[i].size()} <<
      // padding + value + newline
      padding{col_width - col_strs[i].size()} << col_strs[i] << '\n';
  }
  // if series has name, print name
  if (!ser.name().empty())
    out << "Name: " << ser.name() << ", ";
  // if truncated, length also needs to be printed
  if (truncate)
    out << "Length: " << ser.size() << ", ";
  // print data type + finish
  return out << "dtype: " << ser.type();
}

/**
 * Write the `series` to the output stream.
 *
 * This delegates to the table formatter `operator<<` with default options. To
 * stream the `series` with non-default options one can first stream an
 * appropriate `st_format_options`. For example, to disable truncation:
 *
 * @code{.cc}
 * out <<
 *   st_format_options{}
 *     .max_rows({})
 *     .max_cols({}) <<
 *   ser << std::endl;
 * @endcode
 *
 * @param out Output stream
 * @param ser Series to write
 */
inline auto& operator<<(std::ostream& out, const series& ser)
{
  return out << st_format_options{} << ser;
}

}  // namespace oa

#endif  // OA_SERIES_H_
