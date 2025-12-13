/**
 * @file testing/gtest.h
 * @author Derek Huang
 * @brief Google Test compatibility + extension header
 * @copyright MIT License
 *
 * Intended for compatibility with older Google Test versions. Defines some of
 * its own symbols as well for compatibility reasons and provides extensions.
 */

#ifndef OA_TESTING_GTEST_H_
#define OA_TESTING_GTEST_H_

#include <functional>
#include <iomanip>
#include <ostream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>  // for well-formed typeid() usage
#include <utility>

#include <boost/core/demangle.hpp>
#include <gtest/gtest.h>

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
  // print in YYYY/MM/DD ([0-9]+), note std::setw resets after each expression
  *stream << std::setw(4) << date.m_years() << "/" <<
    std::setw(2) << date.m_months() << "/" <<
    std::setw(2) << date.m_days() << " (" << date.GetJulian() << ")";
  // fill character needs to be manually reset
  stream->fill(fill_char);
}

}  // namespace time
}  // namespace oa

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

// TODO: add more binary_format_traits specializations as appropriate

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

/**
 * Format a `::testing::AssertionResult` based on the comparator and values.
 *
 * This supports arbitrary n-ary predicates with some special formatting
 * support for binary predicates like `std::equal_to`.
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
template <bool is_compile_time, typename F, typename... Ts>
requires (std::is_invocable_r_v<bool, F, Ts...>)
auto make_result(bool res, const F& /*comp*/, const Ts&... values)
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
      return ::testing::AssertionFailure() << prefix <<
        v1 << " " << binary_format_traits<F>::op_string << " " << v2;
    };
    return make_failure(values...);
  }
  // otherwise, use generic RTTI-based invocable formatting
  else
    return ::testing::AssertionFailure() << prefix <<
      boost::core::demangle(typeid(F).name()) << "{}(" <<
      delimited{", ", values...} << ")";
}

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
template <bool res, typename F, typename... Ts>
requires (std::is_invocable_r_v<bool, F, Ts...>)
auto make_result(const F& comp, const Ts&... values)
{
  return make_result<true>(res, comp, values...);
}

}  // namespace testing
}  // namespace oa

#endif  // OA_TESTING_GTEST_H_
