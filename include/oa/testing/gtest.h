/**
 * @file testing/gtest.h
 * @author Derek Huang
 * @brief C++ Google Test compatibility + extension header
 * @copyright MIT License
 *
 * Intended for compatibility with older Google Test versions. Defines some of
 * its own symbols as well for compatibility reasons and provides extensions.
 */

#ifndef OA_TESTING_GTEST_H_
#define OA_TESTING_GTEST_H_

#include <concepts>
#include <cstddef>
#include <cstdlib>  // for OA_GTEST_ENSURE_BASE_DIR
#include <functional>
#include <iomanip>
#include <ostream>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>  // for well-formed typeid() usage
#include <utility>

#include <boost/core/demangle.hpp>
#include <gtest/gtest.h>

#include "oa/warnings.h"
// TODO: refactor include path when namespacing is implemented
#include "time/date.h"

// GTEST_SKIP compatibility macro for pre-1.10 Google Test versions
#ifdef GTEST_SKIP
#define OA_GTEST_SKIP() GTEST_SKIP()
#else
#define OA_GTEST_SKIP() GTEST_SUCCESS_("Skipped")
#endif  // GTEST_SKIP

// macros defined for compatibility with older versions of Google Test, which
// often used "case" instead of the more modern "suite" naming convention.

#ifndef INSTANTIATE_TEST_SUITE_P
#define INSTANTIATE_TEST_SUITE_P INSTANTIATE_TEST_CASE_P
#endif  // INSTANTIATE_TEST_SUITE_P

#ifndef TYPED_TEST_SUITE
#define TYPED_TEST_SUITE TYPED_TEST_CASE
#endif  // TYPED_TEST_SUITE

#ifndef TYPED_TEST_SUITE_P
#define TYPED_TEST_SUITE_P TYPED_TEST_CASE_P
#endif  // TYPED_TEST_SUITE_P

#ifndef REGISTER_TYPED_TEST_SUITE_P
#define REGISTER_TYPED_TEST_SUITE_P REGISTER_TYPED_TEST_CASE_P
#endif  // REGISTER_TYPED_TEST_SUITE_P

#ifndef INSTANTIATE_TYPED_TEST_SUITE_P
#define INSTANTIATE_TYPED_TEST_SUITE_P INSTANTIATE_TYPED_TEST_CASE_P
#endif  // INSTANTIATE_TYPED_TEST_SUITE_P

namespace oa {
namespace time {

/**
 * Custom Google Test printer for `oa::time::Date`.
 *
 * The print format is `YYYY/MM/DD ([0-9]+)`, where the integer portion in
 * parentheses is the Julian date integer value of the date object.
 *
 * @param date Date to print
 * @param stream Stream to write to
 */
inline void PrintTo(const Date& date, std::ostream* stream)
{
  // save current stream fill character and set to '0'
  auto fill_char = stream->fill();
  stream->fill('0');
  // get Gregorian date components
  auto [y, m, d] = date.gregorian();
  // print in YYYY/MM/DD ([0-9]+), note std::setw resets after each expression
  *stream <<
    std::setw(4) << y << "/" <<
    std::setw(2) << m << "/" <<
    std::setw(2) << d << " (" << date.julian() << ")";
  // fill character needs to be manually reset
  stream->fill(fill_char);
}

}  // namespace time
}  // namespace oa

namespace oa {
namespace testing {
namespace detail {

/**
 * Return the value of the `OdinBaseDir` environment variable.
 *
 * `nullptr` is returned if `OdinBaseDir` is not defined in the environment.
 *
 * @note This wrapper function around `std::getenv` exists so that use of
 *  `OA_GTEST_ENSURE_BASE_DIR()` will trigger a compiler deprecation warning
 *  due to the `[[deprecated]]` annotation. Do *not* use in new code.
 */
[[deprecated("Use oa::config::data_dir() for the static data directory")]]
inline const char* base_dir()
{
  return std::getenv("OdinBaseDir");
}

}  // namespace detail
}  // namespace testing
}  // namespace oa

/**
 * Macro for skipping a Google Test test if `OdinBaseDir` is not set.
 *
 * Several of the date-related tests require `OdinBaseDir` to be set in the
 * environment and could be skipped if the variable is not defined. Like
 * `GTEST_SKIP` itself this macro can only be used in `SetUp()` members or in
 * the actual Google Test test cases themselves.
 *
 * @deprecated `OdinBaseDir` is currently no longer used to tell Odin libraries
 *  where to find static data, e.g. calendar files. `oa::config::data_dir()`
 *  provides the location; see comment in `oa/config/paths.h` for details.
 */
#define OA_GTEST_ENSURE_BASE_DIR() \
  if (!oa::testing::detail::base_dir()) \
    OA_GTEST_SKIP() << "OdinBaseDir not set in environment"

namespace oa {
namespace testing {

/**
 * Format traits for binary comparators.
 *
 * Eligible binary invocables, e.g. `std::equal_to<>`, can specialize this type
 * with a `static constexpr const char[]` member named `op_string` that
 * represents how the operator should be represented textually.
 *
 * @tparam T Binary comparator
 */
template <typename T>
struct binary_format_traits {};

/**
 * Traits to check if a binary comparator has a valid traits specialization.
 *
 * @tparam T type
 */
template <typename T, typename = void>
struct has_binary_format_traits : std::false_type {};

/**
 * True specialization for a binary comparator with the `op_string` member.
 *
 * @note We could be more detailed in checking `op_string` traits.
 *
 * @tparam T type
 */
template <typename T>
struct has_binary_format_traits<
  T, std::void_t<decltype(binary_format_traits<T>::op_string)> >
  : std::true_type {};

/**
 * Indicate if a binary comparator has a valid traits specialization.
 *
 * @tparam T type
 */
template <typename T>
constexpr bool has_binary_format_traits_v = has_binary_format_traits<T>::value;

/**
 * Format specialization for `std::equal_to`.
 *
 * @tparam T type
 */
template <typename T>
struct binary_format_traits<std::equal_to<T>> {
  static constexpr const char op_string[] = "==";
};

/**
 * Format specialization for `std::not_equal_to`.
 *
 * @tparam T type
 */
template <typename T>
struct binary_format_traits<std::not_equal_to<T>> {
  static constexpr const char op_string[] = "!=";
};

/**
 * Format specialization for `std::less`.
 *
 * @tparam T type
 */
template <typename T>
struct binary_format_traits<std::less<T>> {
  static constexpr const char op_string[] = "<";
};

/**
 * Format specialization for `std::less_equal`.
 *
 * @tparam T type
 */
template <typename T>
struct binary_format_traits<std::less_equal<T>> {
  static constexpr const char op_string[] = "<=";
};

/**
 * Format specialization for `std::greater`.
 *
 * @tparam T type
 */
template <typename T>
struct binary_format_traits<std::greater<T>> {
  static constexpr const char op_string[] = ">";
};

/**
 * Format specialization for `std::greater_equal`.
 *
 * @tparam T type
 */
template <typename T>
struct binary_format_traits<std::greater_equal<T>> {
  static constexpr const char op_string[] = ">=";
};

/**
 * Format multiple values into an output stream using a delimiter.
 *
 * @todo Consider moving this to a dedicated formatting header instead.
 *
 * @tparam Ts Value types
 */
template <typename... Ts>
class delimited {
public:
  /**
   * Ctor.
   *
   * @param delim Value delimiter
   * @param values Values to stream
   */
  constexpr delimited(std::string_view delim, const Ts&... values) noexcept
    : delim_{std::move(delim)}, values_{values...}
  {}

  /**
   * Write the delimited values to the output stream.
   *
   * @param out Output stream
   */
  void write(std::ostream& out) const
  {
    write(std::index_sequence_for<Ts...>{}, out);
  }

private:
  std::string_view delim_;
  std::tuple<const Ts&...> values_;

  /**
   * Write the values to the output stream.
   *
   * @tparam Is Indices 0 through sizeof...(Ts) - 1
   *
   * @param out Output stream
   */
  template <std::size_t... Is>
  void write(std::index_sequence<Is...>, std::ostream& out) const
  {
    static_assert(sizeof...(Is) == sizeof...(Ts));
    (
      [this, &out]
      {
        // write delimiter if not the first value
        if constexpr (Is > 0u)
          out << delim_;
        // write corresponding value
        out << std::get<Is>(values_);
      }(), ...
    );
  }
};

/**
 * Stream a `delimited` object to the given output stream.
 *
 * @tparam Ts Value types
 */
template <typename... Ts>
auto& operator<<(std::ostream& out, const delimited<Ts...>& value)
{
  value.write(out);
  return out;
}

namespace detail {

/**
 * Format a `::testing::AssertionResult` based on the comparator and values.
 *
 * This implements `::testing::AssertionResult` formatting for for arbitrary
 * n-ary predicates that do not return a `::testing::AssertionResult`.
 *
 * @note This overload does not invoke the comparator at all and relies on the
 *  `res` value which should be the result of `comp(values...)`.
 *
 * @tparam is_compile_time `true` if `comp(values...)` invoked at compile time
 * @tparam F n-ary comparator
 * @tparam Ts Value types
 *
 * @param res Result of `comp(values...)`
 * @param values Values comparator was invoked with
 */
template <bool is_compile_time, typename R, typename F, typename... Ts>
requires (std::is_invocable_r_v<R, F, Ts...> && std::is_convertible_v<R, bool>)
auto make_result(R res, const F& /*comp*/, const Ts&... values)
{
  // success, so no error message formatting
  if (res)
    return ::testing::AssertionSuccess();
  // constexpr message prefix
  constexpr auto prefix = []
  {
    if constexpr (is_compile_time)
      return "[constexpr] ";
    else
      return "";
  }();
  // if binary invocable with formatting traits
  if constexpr (has_binary_format_traits_v<F>) {
    // assuming that the invocable really is a binary invocable
    auto make_failure = [prefix](const auto& v1, const auto& v2)
    {
      return ::testing::AssertionFailure() << "result of: " << prefix <<
        v1 << " " << binary_format_traits<F>::op_string << " " << v2;
    };
    return make_failure(values...);
  }
  // otherwise, use generic RTTI-based invocable formatting
  else
    return ::testing::AssertionFailure() << "result of: " << prefix <<
      boost::core::demangle(typeid(F).name()) << "{}(" <<
      delimited{", ", values...} << ")";
}

}  // namespace detail

// suppress C4100 to prevent explosion in messages per template instance
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4100)
/**
 * Format a `::testing::AssertionResult` based on the comparator and values.
 *
 * This supports arbitrary n-ary predicates with some special formatting
 * support for binary predicates like `std::equal_to`. If `comp(values...)`
 * returns a `::testing::AssertionResult` the object is passed through.
 *
 * @note This overload does not invoke the comparator at all and relies on the
 *  `res` value which should be the result of `comp(values...)`.
 *
 * @tparam is_compile_time `true` if `comp(values...)` invoked at compile time
 * @tparam F n-ary comparator
 * @tparam Ts Value types
 *
 * @param res Result of `comp(values...)`
 * @param values Values comparator was invoked with
 */
template <bool is_compile_time, typename R, typename F, typename... Ts>
requires (std::is_invocable_r_v<R, F, Ts...> && std::is_convertible_v<R, bool>)
auto make_result(R res, const F& comp, const Ts&... values)
{
  // if AssertionResult itself just pass through
  if constexpr (std::is_same_v<R, ::testing::AssertionResult>)
    return std::move(res);
  // otherwise check binary_format_traits<> and do generic formatting
  else
    return detail::make_result<is_compile_time>(std::move(res), comp, values...);
}
OA_MSVC_WARNING_POP()

/**
 * Format a `::testing::AssertionResult` based on the comparator and values.
 *
 * This supports arbitrary n-ary predicates with some special formatting
 * support for binary predicates like `std::equal_to` and is the overload that
 * should be used for runtime invocation of `comp(values...)`.
 *
 * @tparam F n-ary comparator
 * @tparam Ts Value types
 *
 * @param comp Comparator object
 * @param values Values to invoke comparator on
 */
template <typename F, typename... Ts>
requires (std::is_invocable_r_v<bool, F, Ts...>)
auto make_result(const F& comp, const Ts&... values)
{
  return make_result<false>(comp(values...), comp, values...);
}

/**
 * Format a `::testing::AssertionResult` based on the comparator and values.
 *
 * This supports arbitrary n-ary predicates with some special formatting
 * support for binary predicates like `std::equal_to` and is the overload that
 * should be used for compile-time invocation of `comp(values...)`.
 *
 * @tparam res Result of `comp(values...)`
 * @tparam F n-ary comparator
 * @tparam Ts Value types
 *
 * @param comp Comparator object
 * @param values Values to invoke comparator on
 */
template <auto res, typename F, typename... Ts>
requires (std::is_invocable_r_v<decltype(res), F, Ts...>)
auto make_result(const F& comp, const Ts&... values)
{
  return make_result<true>(res, comp, values...);
}

namespace detail {

/**
 * Concept for a string-like object.
 *
 * We simply check that the object is a contiguous range of char type.
 *
 * @note Unused in this header but could be useful in a formatting context.
 *
 * @tparam T type
 */
template <typename T>
concept string_like = (
  std::ranges::contiguous_range<T> &&
  std::same_as<std::ranges::range_value_t<T>, char>
);

/**
 * Concept for weak equality in one direction.
 *
 * This is a weaker form of `std::equality_comparable`.
 *
 * @tparam T First type
 * @tparam U Second type
 */
template <typename T, typename U>
concept weakly_equality_comparable = requires (T x, U y) {
  { x == y } -> std::convertible_to<bool>;
};

}  // namespace detail

/**
 * Check that two values are equal and return a `::testing::AssertionResult`.
 *
 * This is useful for simple `x == y` testing with a default error message.
 *
 * @tparam T First type
 * @tparam U Second type
 *
 * @param x First value
 * @param y Second value
 */
template <typename T, typename U>
requires (detail::weakly_equality_comparable<T, U>)
auto eq(const T& x, const U& y)
{
  // perform equality check
  if (x == y)
    return ::testing::AssertionSuccess();
  else
    return ::testing::AssertionFailure() << x << " != " << y;
}

/**
 * Compile-time index type.
 *
 * This is useful for specializing `::testing::Test<T>` where `T` is a type
 * that holds an integral value. The reason we don't use the standard library
 * `std::integral_constantstd::size_t, I>` or `std::index_sequence<I>` is due
 * to both of these options having longer, more verbose type names.
 *
 * @tparam I Index value
 */
template <std::size_t I>
class index {
public:
  static constexpr auto value = I;

  /**
   * Implicitly convert to the non-type type parameter value.
   */
  constexpr operator std::size_t() const noexcept
  {
    return I;
  }

  /**
   * Return the non-type type parameter value.
   */
  constexpr auto operator()() const noexcept
  {
    return I;
  }
};

/**
 * Traits type to construct a `::testing::Types<...>` from another type.
 *
 * Typically the `T` is a `std::index_sequence<Is...>` where each `Is` indexes
 * into an input array and each test template input type is
 * `std::index_sequence<Is>` to index the input array.
 *
 * @tparam T `std::index_sequence<...>`
 */
template <typename T>
struct test_types {};

/**
 * Partial specialization for a `std::index_sequence<Is...>`.
 *
 * This is commonly used in the aforementioned scenario where each GoogleTest
 * test template is instantiated with an `oa::testing::index<Is>`, where each
 * `Is` is in index into an array of inputs.
 *
 * @tparam Is Indices 0 through the size of the input range
 */
template <std::size_t... Is>
struct test_types<std::index_sequence<Is...>> {
  using type = ::testing::Types<index<Is>...>;
};

/**
 * SFINAE helper to get the `::testing::Types<...>` from `test_types<T>`.
 *
 * @tparam T type
 */
template <typename T>
using test_types_t = typename test_types<T>::type;

/**
 * Traits type for a `::testing::Types<...>` of `oa::testing::index<Is>`.
 *
 * @tparam N Number of test inputs
 */
template <std::size_t N>
using index_types = test_types_t<std::make_index_sequence<N>>;

}  // namespace testing
}  // namespace oa

#endif  // OA_TESTING_GTEST_H_
