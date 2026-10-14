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
#include <utility>
#include <variant>
#include <vector>

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
        throw std::runtime_error{"cannot address buffer with specified type"};
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
        throw std::runtime_error{"cannot convert std::monostate to target type"};
      // can't do anything with non-convertible types
      // note: can use to_series_type<U>() to support remaining types
      else
        throw std::runtime_error{
          "cannot convert " + std::string{to_string(to_series_type<U>())} +
          " to target type"
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
    std::convertible_to<std::ranges::range_value_t<R>, series_object> ||
    std::is_same_v<std::ranges::range_value_t<R>, long>;

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
     *
     * @param name Name of type to show in exception
     */
    template <typename T>
    requires (std::is_arithmetic_v<T>)
    auto as(const char* name = "T") const
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
          "cannot convert " + std::string{to_string(type())} + " to " + name
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
      return as<int>("int");
    }

    /**
     * Implicitly convert to `long`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator long() const
    {
      return as<long>("long");
    }

    /**
     * Implicitly convert to `long long`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator long long() const
    {
      return as<long long>("long long");
    }

    /**
     * Implicitly convert to a `float`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator float() const
    {
      return as<float>("float");
    }

    /**
     * Implicitly convert to a `double`.
     *
     * If the proxied value cannot be converted an exception is thrown.
     */
    operator double() const
    {
      return as<double>("double");
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

    /**
     * Assign a new value through the value proxy.
     *
     * This member is conditionally enabled when the owning series can be
     * modified and when the parent `series` is non-const.
     *
     * @note Member is rvalue reference qualified to disable value modification
     *  through a direct-initialized instance of the `value_proxy`.
     *
     * @tparam T type
     */
    template <typename T>
    requires (!std::is_const_v<S>)
    auto& operator=(T&& new_val) &&
    {
      // get "closest" series type for T
      using U = closest_series_type_t<std::decay_t<T>>;
      // if type matches we can set directly
      // note: enable explicit conversions by calling list-init
      if (type() == to_series_type<U>()) {
        value<U>() = U{std::forward<T>(new_val)};
        return *this;
      }
      // otherwise re-allocate + modify new series storage
      // note: enable explicit conversions by calling list-init
      std::vector<U> new_data(s_->size());
      new_data[i_] = U{std::forward<T>(new_val)};
      s_->data_ = std::move(new_data);
      return *this;
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

// TODO: formatting options + padding helpers should move to st_format.h

// TODO: document
class st_format {
public:
  /**
   * Default ctor.
   */
  st_format() noexcept = default;

  /**
   * Return the thread-local `st_format` instance.
   */
  static auto& instance() noexcept
  {
    thread_local st_format fmt;
    return fmt;
  }

  /**
   * Set the max number of rows to print.
   */
  static auto& max_rows(std::optional<std::size_t> v) noexcept
  {
    instance().max_rows_ = std::move(v);
    return instance();
  }

  /**
   * Get the max number of rows to print.
   *
   * If unset then there is no limit to the number of printed rows.
   */
  static auto& max_rows() noexcept
  {
    return instance().max_rows_;
  }

  /**
   * Set the max number of columns to print.
   */
  static auto& max_cols(std::optional<std::size_t> v) noexcept
  {
    instance().max_cols_ = std::move(v);
    return instance();
  }

  /**
   * Get the max number of columns to print.
   *
   * If unset, a truncated view of the columns is printed.
   *
   * @todo Consider if we should detect the terminal width for this.
   */
  static auto& max_cols() noexcept
  {
    return instance().max_cols_;
  }

  /**
   * Indicate if truncation is needed given the number of rows to print.
   *
   * Returns `true` if `n` exceeds the greater of 10 or `max_rows()`.
   *
   * @param n Number of rows to print
   */
  static bool truncate_rows(std::size_t n) noexcept
  {
    // no limit, so no truncation
    if (!max_rows())
      return false;
    // otherwise, return result of comparison
    return n > (std::max)(std::size_t{10u}, *max_rows());
  }

private:
  std::optional<std::size_t> max_rows_{50u};
  std::optional<std::size_t> max_cols_;
};

/**
 * RAII `st_format` context class.
 *
 * This class can have its members set to modify the thread-local `st_format`
 * settings, restoring the original formatting settings on destruction.
 */
class st_format_context {
public:
  /**
   * Default ctor.
   *
   * The formatting settings are populated using `st_format::instance()`.
   */
  st_format_context() noexcept : fmt_{st_format::instance()} {}

  /**
   * Dtor.
   *
   * Reset the formatting settings to the previously captured settings.
   */
  ~st_format_context()
  {
    st_format::instance() = fmt_;
  }

  /**
   * Set the max number of rows to print.
   */
  auto& max_rows(std::optional<std::size_t> v) noexcept
  {
    st_format::max_rows(std::move(v));
    return *this;
  }

  /**
   * Get the max number of rows to print.
   */
  auto& max_rows() const noexcept
  {
    return st_format::max_rows();
  }

  /**
   * Set the max number of columns to print.
   */
  auto& max_cols(std::optional<std::size_t> v) noexcept
  {
    st_format::max_cols(std::move(v));
    return *this;
  }

  /**
   * Get the max number of columns to print.
   */
  auto& max_cols() const noexcept
  {
    return st_format::max_cols();
  }

private:
  st_format fmt_;
};

/**
 * Class representing an amount of whitespace padding.
 */
class padding {
public:
  /**
   * Ctor.
   *
   * @param n Number of padding characters
   */
  constexpr padding(std::size_t n) noexcept : n_{n} {}

  /**
   * Return the padding width.
   */
  constexpr auto size() const noexcept { return n_; }

private:
  std::size_t n_;
};

/**
 * Write the specified amount of whitespace padding to the output stream.
 *
 * @param out Output stream
 * @param pad Padding specification
 */
inline auto& operator<<(std::ostream& out, const padding& pad)
{
  for (std::size_t i = 0u; i < pad.size(); i++)
    out.put(' ');
  return out;
}

/**
 * Write the `series` to the output stream.
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
 * @param out Output stream
 * @param ser Series to write
 */
inline auto& operator<<(std::ostream& out, const series& ser)
{
  // indicate if truncated representation is printed
  bool truncate = st_format::truncate_rows(ser.size());
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
    // index + padding + separating 4 spaces
    out << index_strs[i] << padding{4u + index_width - index_strs[i].size()};
    // padding + value + newline
    out << padding{col_width - col_strs[i].size()} << col_strs[i] << '\n';
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

}  // namespace oa

#endif  // OA_SERIES_H_
