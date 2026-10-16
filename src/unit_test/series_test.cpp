/**
 * @file series_test.cpp
 * @author Derek Huang
 * @brief series.h unit tests
 * @copyright MIT License
 */

#include "oa/series.h"

#include <concepts>
#include <cstdint>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "oa/common.h"         // OA_IDENTITY()
#include "oa/testing/gtest.h"  // oa::testing::merge_test_types_t

namespace {

/**
 * `series` template test suite.
 *
 * The base `void` specialization serves as a concrete test fixture.
 *
 * @tparam T Input test type
 */
template <typename T = void>
class SeriesTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////

/**
 * Template type for `closest_series_type` tests.
 *
 * @tparam T Input type
 * @tparam U Expected output type
 */
template <typename T, typename U>
struct closest_type_test {};

/**
 * Partial specialization for `closest_series_type` tests.
 *
 * @tparam T Input type
 * @tparam U Expected output type
 */
template <typename T, typename U>
class SeriesTest<closest_type_test<T, U>> : public SeriesTest<> {
protected:
  /**
   * Check that `closest_series_type<T>` works as expected.
   */
  void operator()() const
  {
    EXPECT_TRUE((std::is_same_v<U, oa::closest_series_type_t<T>>));
  }
};

////////////////////////////////////////////////////////////////////////////////

/**
 * Template type for `series` initializer list tests.
 *
 * This is used only to disambiguate test case indexing.
 *
 * @tparam I Input index
 */
template <std::size_t I>
struct series_init_test {};

/**
 * `series` initializer list test input.
 *
 * To allow `series` initialization manually, e.g. with `oa::series{1, 2, 3}`,
 * we allow use of a function pointer, which a lambda with no captures can
 * convert to, as the `series` creation function.
 */
class series_init_test_input {
public:
  /**
   * Ctor.
   *
   * The series name is simply the empty string.
   *
   * @param size Expected `series` size
   * @param type Expected `series` type
   * @param f `series` creation function
   */
  constexpr series_init_test_input(
    std::size_t size,
    oa::series_type type,
    oa::series (*f)()) noexcept : series_init_test_input{size, type, "", f}
  {}

  /**
   * Ctor.
   *
   * @param size Expected `series` size
   * @param type Expected `series` type
   * @param name Expected `series` name
   * @param f `series` creation function
   */
  constexpr series_init_test_input(
    std::size_t size,
    oa::series_type type,
    std::string_view name,
    oa::series (*f)()) noexcept
    : size_{size}, type_{type}, name_{name}, f_{f}
  {}

  /**
   * Create the input `series`.
   */
  auto operator()() const
  {
    return f_();
  }

  // getters
  constexpr auto size() const noexcept { return size_; }
  constexpr auto type() const noexcept { return type_; }
  constexpr auto name() const noexcept { return name_; }

private:
  std::size_t size_;       // expected series size
  oa::series_type type_;   // expected series type
  std::string_view name_;  // expected series name
  oa::series (*f_)();      // function to create the series
};

// series_init_test test case inputs
constexpr series_init_test_input series_init_test_inputs[] = {
  {4, oa::series_type::int32, [] { return oa::series{1, 2, 3, 4}; }},
  {3, oa::series_type::int64, [] { return oa::series{1ll, 2ll, 4ll}; }},
  {3, oa::series_type::float32, [] { return oa::series{1.f, 3.f, 5.f}; }},
  // note: unsigned types are converted to double
  {5, oa::series_type::float64, [] { return oa::series{2u, 3u, 5u, 6u, 7u}; }},
  {3, oa::series_type::float64, [] { return oa::series{2., 4., 6.}; }},
  {2, oa::series_type::string, [] { return oa::series{"hello", "world"}; }},
  // variadic initialization
  {3, oa::series_type::object, [] { return oa::series{3, 3.4, "hello"}; }},
  // init with name
  {2, oa::series_type::float64, "a", [] { return oa::series{"a", {1., 2.}}; }},
  {2, oa::series_type::int32, "b", [] { return oa::series{"b", {4, 4}}; }},
  {3, oa::series_type::object, "c", [] { return oa::series{"c", {1, 2., "h"}}; }},
  {2, oa::series_type::float32, "d", [] { return oa::series{"d", {1.f, 2.f}}; }}
};

/**
 * Partial specialization for `series_init_test` tests.
 *
 * @tparam I Input index
 */
template <std::size_t I>
class SeriesTest<series_init_test<I>> : public SeriesTest<> {
protected:
  // ensure we don't index out of bounds
  static_assert(I < std::size(series_init_test_inputs));

  /**
   * Test that a `series` is correctly created from an initializer list.
   */
  void operator()() const
  {
    // input case
    constexpr auto& input = series_init_test_inputs[I];
    // create series + check
    auto series = input();
    EXPECT_EQ(input.size(), series.size());
    EXPECT_EQ(input.type(), series.type());
    EXPECT_EQ(input.name(), series.name());
  }
};

/**
 * Traits type to create a `::testing::Types<...>` for `series_init_test`.
 *
 * @tparam T type
 */
template <typename T>
struct series_init_test_types {};

/**
 * Partial specialization for a `std::index_sequence<Is...>`.
 *
 * @tparam Is Indices 0 through `std::size(series_init_test_inputs)` - 1
 */
template <std::size_t... Is>
struct series_init_test_types<std::index_sequence<Is...>> {
  using type = ::testing::Types<series_init_test<Is>...>;
};

/**
 * `void` specialization using the size of `series_init_test_inputs`.
 */
template <>
struct series_init_test_types<void> : series_init_test_types<
  std::make_index_sequence<std::size(series_init_test_inputs)> > {};

/**
 * Type alias for the `::testing::Types<series_init_test<Is>...>` types.
 */
using series_init_test_types_t = typename series_init_test_types<void>::type;

////////////////////////////////////////////////////////////////////////////////

/**
 * Template type for `series` iterator traits tests.
 *
 * This is used only to disambiguate test case indexing.
 *
 * @tparam I Input index
 */
template <std::size_t I>
struct series_iter_traits_test {};

// helpers to shorten the iterator type names
using series_iterator = oa::series::iterator<oa::series>;
using const_series_iterator = oa::series::iterator<const oa::series>;

/**
 * Macro for a `series` iterator traits test input.
 *
 * This helps with error messages as the arguments are stringified.
 *
 * @param ... Arguments
 */
#define OA_STRING_EXPR(...) __VA_ARGS__, #__VA_ARGS__

// series_iter_traits_test test case inputs
// note: we test the weaker iterator concepts too to help with debugging
// note: the std::pair second element is used for the test error message
constexpr std::pair<bool, const char*> series_iter_traits_test_inputs[] = {
  {OA_STRING_EXPR(std::input_iterator<series_iterator>)},
  {OA_STRING_EXPR(std::input_iterator<const_series_iterator>)},
  {OA_STRING_EXPR(std::forward_iterator<series_iterator>)},
  {OA_STRING_EXPR(std::forward_iterator<const_series_iterator>)},
  {OA_STRING_EXPR(std::bidirectional_iterator<series_iterator>)},
  {OA_STRING_EXPR(std::bidirectional_iterator<const_series_iterator>)},
  {OA_STRING_EXPR(std::random_access_iterator<const_series_iterator>)},
  {OA_STRING_EXPR(std::random_access_iterator<const_series_iterator>)}
};

/**
 * Partial specialization for `series` iterator traits tests.
 *
 * @tparam I Input index
 */
template <std::size_t I>
class SeriesTest<series_iter_traits_test<I>> : public SeriesTest<> {
protected:
  // ensure we don't index out of bounds
  static_assert(I < std::size(series_iter_traits_test_inputs));

  /**
   * Test that the `series` iterator satisfies the concept.
   */
  void operator()() const
  {
    auto [res, err] = series_iter_traits_test_inputs[I];
    EXPECT_TRUE(res) << "Concept not satisfied: " << err;
  }
};

/**
 * Traits type to create the `series_iter_traits_test` test types.
 *
 * @tparam T type
 */
template <typename T>
struct series_iter_traits_test_types {};

/**
 * Partial specialization for a `std::index_sequence<Is...>`.
 *
 * @tparam Is Indices 0 through `std::size(series_iter_traits_test_inputs)` - 1
 */
template <std::size_t... Is>
struct series_iter_traits_test_types<std::index_sequence<Is...>> {
  using type = ::testing::Types<series_iter_traits_test<Is>...>;
};

/**
 * `void` specialization using the size of `series_iter_traits_test_inputs`.
 */
template <>
struct series_iter_traits_test_types<void> : series_iter_traits_test_types<
  std::make_index_sequence<std::size(series_iter_traits_test_inputs)> > {};

/**
 * Types alias for `::testing::Types<series_iter_traits_test<Is>...>`.
 */
using series_iter_traits_test_types_t =
  typename series_iter_traits_test_types<void>::type;

////////////////////////////////////////////////////////////////////////////////

// TODO: add iteration tests here

////////////////////////////////////////////////////////////////////////////////

/**
 * Template type for `series` value proxy tests.
 *
 * This is used only to disambiguate test case indexing.
 *
 * @tparam I Input index
 */
template <std::size_t I>
struct series_value_proxy_test {};

/**
 * Value wrapper for `series` value proxy tests for `series_object` conversion.
 *
 * Since persistently allocated `std::string` is not `constexpr`, when we
 * explicitly want to construct a `series_object`, we do this in a `constexpr`
 * branch in the test itself for a runtime `series_object` manipulation.
 *
 * @tparam T type
 */
template <typename T>
struct series_object_proxy {
  T value;
};

/**
 * Traits type for detecting a `series_object_proxy`.
 *
 * @tparam T type
 */
template <typename T>
struct is_series_object_proxy : std::false_type {};

/**
 * Partial specialization for a `series_object_proxy`.
 *
 * @tparam T type
 */
template <typename T>
struct is_series_object_proxy<series_object_proxy<T>> : std::true_type {};

/**
 * Indicate if a type is a `series_object_proxy`.
 *
 * @tparam T type
 */
template <typename T>
constexpr bool is_series_object_proxy_v = is_series_object_proxy<T>::value;

// series_value_proxy_test test case inputs
// note: only add values that have alue_proxy conversion operators
constexpr auto series_value_proxy_test_inputs = std::make_tuple(
  12,
  3.,
  "hello",
// note: MSVC has 32-bit long which doesn't convert to series_object
#ifndef _MSC_VER
  16l,
#endif  // _MSC_VER
  15ll,
  3.14159f,
  series_object_proxy{155},
  series_object_proxy{"tonight"},
  series_object_proxy{2.6667},
  series_object_proxy{9.8f},
// note: MSVC has 32-bit long which doesn't convert to series_object
#ifndef _MSC_VER
  series_object_proxy{12l},
#endif  // _MSC_VER
  series_object_proxy{22ll}
);

/**
 * Partial specialization for `series` value proxy tests.
 *
 * @tparam I Input index
 */
template <std::size_t I>
class SeriesTest<series_value_proxy_test<I>> : public SeriesTest<> {
protected:
  // type alias + bounds check
  // note: need std::decay_t to remove element type const-qualification
  using inputs_type = std::decay_t<decltype(series_value_proxy_test_inputs)>;
  static_assert(I < std::tuple_size_v<inputs_type>);

  /**
   * Test that `series` value proxy works as expected.
   *
   * A `series` is created with a single element (the input element), indexed
   * to obtain the `value_proxy`, and said proxy is compared to the input value
   * both before and after value conversion to test both `operator==`.
   */
  void operator()() const
  {
    // input value type + input value
    // note: issues between GCC/MSVC over constexpr auto& vs. auto and capture
    using in_type = std::tuple_element_t<I, inputs_type>;
    auto& in = std::get<I>(series_value_proxy_test_inputs);
    // unwrap value if in series_object_proxy
    auto v = [&in]
    {
      if constexpr (is_series_object_proxy_v<in_type>)
        return in.value;
      else
        return in;
    }();
    // create series. if series_object_proxy first convert to series_object
    auto s = [&in]() -> oa::series
    {
      if constexpr (is_series_object_proxy_v<in_type>)
        return {oa::series_object{in.value}};
      else
        return {in};
    }();
    // check equality directly using oa::series::operator==
    EXPECT_EQ(v, s[0]);
    // determine conversion value type. this is decltype(v) unless const char*
    // in which case we directly convert to std::string instead
    using value_type = std::conditional_t<
      std::is_same_v<decltype(v), const char*>, std::string, decltype(v)
    >;
    // check equality post-conversion to value type
    // note: this uses the native operator== for the type
    value_type s0 = s[0];
    EXPECT_EQ(v, s0);
  }
};

/**
 * Traits type to create the `series_value_proxy_test` test types.
 *
 * @tparam T type
 */
template <typename T>
struct series_value_proxy_test_types {};

/**
 * Partial specialization for a `std::index_sequence<Is...>`.
 *
 * @tparam Is Indices 0 through `std::size(series_value_proxy_test_inputs)` - 1
 */
template <std::size_t... Is>
struct series_value_proxy_test_types<std::index_sequence<Is...>> {
  using type = ::testing::Types<series_value_proxy_test<Is>...>;
};

/**
 * `void` specialization using the size of `series_value_proxy_test_inputs`.
 */
template <>
struct series_value_proxy_test_types<void> : series_value_proxy_test_types<
  std::make_index_sequence<
    std::tuple_size_v<decltype(series_value_proxy_test_inputs)>
    > > {};

/**
 * Types alias for `::testing::Types<series_value_proxy_test<Is>...>`.
 */
using series_value_proxy_test_types_t =
  typename series_value_proxy_test_types<void>::type;

////////////////////////////////////////////////////////////////////////////////

/**
 * Template type for `series` `operator<<` tests.
 *
 * This is used only to disambiguate test case indexing.
 *
 * @tparam I Input index
 */
template <std::size_t I>
struct series_stream_test {};

/**
 * `series` `operator<<` input type.
 *
 * Just like the `series_init_test_input` this enables flexible initialization
 * of the `series` via function pointer which is convertible from a lambda.
 */
class series_stream_test_input {
public:
  /**
   * Ctor.
   *
   * @param f `series` creation function
   * @param s Expected `operator<<` output
   */
  constexpr series_stream_test_input(
    oa::series (*f)(),
    std::string_view expected) noexcept
    : f_{f}, expected_{expected}
  {}

  /**
   * Create the input `series`.
   */
  auto operator()() const
  {
    return f_();
  }

  /**
   * Return the expected `operator<<` output.
   */
  auto expected() const noexcept { return expected_; }

private:
  oa::series (*f_)();
  std::string_view expected_;
};

// series_stream_test inputs
constexpr series_stream_test_input series_stream_test_inputs[] = {
  {
    [] { return oa::series{2, 4, 6, 8, 10}; },
    "0     2\n"
    "1     4\n"
    "2     6\n"
    "3     8\n"
    "4    10\n"
    "dtype: int32"
  },
  {
    [] { return oa::series{1., 33., 555., 7777., 99999.}; },
    "0        1\n"
    "1       33\n"
    "2      555\n"
    "3     7777\n"
    "4    99999\n"
    "dtype: float64"
  },
  {
    [] { return oa::series{1.f, 23.4f, 3.14159f, 2.781f}; },
    "0          1\n"
    "1       23.4\n"
    "2    3.14159\n"
    "3      2.781\n"
    "dtype: float32"
  },
  {
    [] { return oa::series{"f1", {3., 4., 5., 6., 7.}}; },
    "0    3\n"
    "1    4\n"
    "2    5\n"
    "3    6\n"
    "4    7\n"
    "Name: f1, dtype: float64"
  },
  {
    [] { return oa::series{"the", "quick", "brown", "fox", "jumped"}; },
    "0       the\n"
    "1     quick\n"
    "2     brown\n"
    "3       fox\n"
    "4    jumped\n"
    "dtype: string"
  },
  {
    [] { return oa::series{1, 4., 4.35, 123ll}; },
    "0       1\n"
    "1       4\n"
    "2    4.35\n"
    "3     123\n"
    "dtype: object"
  },
  {
    [] { return oa::series{"lls", {1ll, 22ll, 333ll, 44ll, 5ll}}; },
    "0      1\n"
    "1     22\n"
    "2    333\n"
    "3     44\n"
    "4      5\n"
    "Name: lls, dtype: int64"
  },
  // truncation + convertible storage test
  {
    []
    {
      // fill vector exceeding max print rows
      std::vector<float> vs(100u);
      for (auto i = 0u; i < vs.size(); i++)
        vs[i] = 2.141f * (i + 1);
      // move into storage
      return oa::series{"movef", std::move(vs)};
    },
    "0       2.141\n"
    "1       4.282\n"
    "2       6.423\n"
    "3       8.564\n"
    "4      10.705\n"
    "         ... \n"
    "95    205.536\n"
    "96    207.677\n"
    "97    209.818\n"
    "98    211.959\n"
    "99      214.1\n"
    "Name: movef, Length: 100, dtype: float32"
  }
};

/**
 * Partial specialization for `series` `operator<<` tests.
 *
 * @tparam I Input index
 */
template <std::size_t I>
class SeriesTest<series_stream_test<I>> : public SeriesTest<> {
protected:
  static_assert(I < std::size(series_stream_test_inputs));

  /**
   * Stream the `series` to a `std::stringstream` and check the output.
   */
  void operator()() const
  {
    constexpr auto& input = series_stream_test_inputs[I];
    std::stringstream ss;
    ss << input();
    // note: view() new in C++20
    EXPECT_EQ(input.expected(), ss.view());
  }
};

/**
 * Traits type to create the `series_stream_test` test types.
 *
 * @tparam T type
 */
template <typename T>
struct series_stream_test_types {};

/**
 * Partial specialization for a `std::index_sequence<Is...>`.
 *
 * @tparam Is Indices 0 through `std::size(series_stream_test_inputs)` - 1
 */
template <std::size_t... Is>
struct series_stream_test_types<std::index_sequence<Is...>> {
  using type = ::testing::Types<series_stream_test<Is>...>;
};

/**
 * `void` specialization using the size of `series_stream_test_inputs`.
 */
template <>
struct series_stream_test_types<void> : series_stream_test_types<
  std::make_index_sequence<std::size(series_stream_test_inputs)> > {};

/**
 * Types alias for `::testing::Types<series_stream_test<Is>...>`.
 */
using series_stream_test_types_t = typename series_stream_test_types<void>::type;

////////////////////////////////////////////////////////////////////////////////

/**
 * Template type for `series` modification + reallocation tests.
 *
 * This is used only to disambiguate test case indexing.
 *
 * @tparam I Input index
 */
template <std::size_t I>
struct series_modify_test {};

// series_modify_test inputs. each input is a (index, value, type, initfunc)
// tuple, where the series returned by initfunc is assigned value at the index
// and the assigned value + the resulting series type are checked
constexpr auto series_modify_test_inputs = std::make_tuple(
  std::tuple{
    2u, 14., oa::series_type::float64,
    [] { return oa::series{1, 2, 3, 4, 5}; }
  },
  std::tuple{
    3u, "hello world", oa::series_type::object,
    [] { return oa::series{1., 3., 5., 7., 9.}; }
  },
  std::tuple{
    2u, 123, oa::series_type::int64,
    [] { return oa::series{2ll, 4ll, 6ll, 8ll}; }
  },
  std::tuple{
    1u, 111ll, oa::series_type::float64,
    [] { return oa::series{3., 1., 4., 1., 5., 9.}; }
  },
  std::tuple{
    2u, 222ll, oa::series_type::float32,
    [] { return oa::series{1.f, 22.f, 333.f, 4444.f}; }
  },
  // TODO: may be surprising to end users
  std::tuple{
    4u, 2.71, oa::series_type::float64,
    [] { return oa::series{1.f, 2.f, 3.f, 4.f, 5.f}; }
  }
);

/**
 * Partial specialization for `series` modification + reallocation tests.
 *
 * @tparam I Input index
 */
template <std::size_t I>
class SeriesTest<series_modify_test<I>> : public SeriesTest<> {
protected:
  static_assert(I < std::tuple_size_v<decltype(series_modify_test_inputs)>);

  /**
   * Check that the modified `series` has the expected type and modified value.
   */
  void operator()() const
  {
    // unpack input
    auto [i, v, type, f] = std::get<I>(series_modify_test_inputs);
    // create series + modify value
    auto s = f();
    s[i] = v;
    // check if series type + modified value is correct
    EXPECT_EQ(type, s.type());
    EXPECT_EQ(v, s[i]);
  }
};

/**
 * Traits type to create the `series_modify_test` test types.
 *
 * @tparam T type
 */
template <typename T>
struct series_modify_test_types {};

/**
 * Partial specialization for a `std::index_sequence<Is...>`.
 *
 * @tparam Is Indices 0 through `std::size(series_modify_test_inputs)` - 1
 */
template <std::size_t... Is>
struct series_modify_test_types<std::index_sequence<Is...>> {
  using type = ::testing::Types<series_modify_test<Is>...>;
};

/**
 * `void` specialization using the size of `series_modify_test_inputs`.
 */
template <>
struct series_modify_test_types<void> : series_modify_test_types<
  std::make_index_sequence<
    std::tuple_size_v<decltype(series_modify_test_inputs)>
  > > {};

/**
 * Types alias for `::testing::Types<series_modify_test<Is>...>`.
 */
using series_modify_test_types_t = typename series_modify_test_types<void>::type;

////////////////////////////////////////////////////////////////////////////////

// input types
TYPED_TEST_SUITE(
  SeriesTest,
  OA_IDENTITY(
    oa::testing::merge_test_types_t<
      ::testing::Types<
        closest_type_test<int, std::int32_t>,
        closest_type_test<short, std::int32_t>,
        // note: on Windows, long is 32 bits, so we have a conditional check.
        // we don't actually support long, however, as it cannot convert to
        // series_object correctly despite support for long long (int64_t)
        closest_type_test<
          long,
          std::conditional_t<
            sizeof(long) <= sizeof(std::int32_t),
            std::int32_t,
            std::int64_t
          >
        >,
        closest_type_test<long long, std::int64_t>,
        // note: unsigned integral types are allocated as double
        closest_type_test<unsigned short, double>,
        closest_type_test<unsigned, double>,
        closest_type_test<float, float>,
        closest_type_test<double, double>,
        closest_type_test<const char*, std::string>,
        closest_type_test<const char (&)[10], std::string>,
        // note: series_object is catch-all even if conversion is not possible
        closest_type_test<oa::series_object, oa::series_object>,
        closest_type_test<std::pair<int, int>, oa::series_object>
      >,
      series_init_test_types_t,
      series_iter_traits_test_types_t,
      series_value_proxy_test_types_t,
      series_stream_test_types_t,
      series_modify_test_types_t
    >
  )
);

/**
 * Evaluate the test case.
 */
TYPED_TEST(SeriesTest, Test)
{
  (*this)();
}

}  // namespace
