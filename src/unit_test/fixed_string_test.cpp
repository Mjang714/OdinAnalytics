/**
 * @file fixed_string_test.cpp
 * @author Derek Huang
 * @brief fixed_string.h unit tests
 * @copyright MIT License
 */

#include "oa/fixed_string.h"

#include <cstddef>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "oa/testing/gtest.h"

namespace {

// note: test case names a bit more verbose due to use of unity build. using
// OA_UNITY_ID has been discarded due to it affecting the test case name and
// also requiring a second anonymous namespace

/**
 * `fixed_string` binary test case.
 *
 * This holds two `fixed_string` instances as well as the corresponding binary
 * comparison operator that will be used to do the comparison.
 *
 * @tparam F Binary comparator
 * @tparam N1 First string length
 * @tparam N2 Second string length
 */
template <typename F, std::size_t N1, std::size_t N2>
class fixed_string_binary_test_case {
public:
  /**
   * Ctor.
   *
   * @param op Binary comparator
   * @param s1 First fixed string
   * @param s2 Second fixed string
   */
  constexpr fixed_string_binary_test_case(
    F&& op,
    const oa::fixed_string<N1>& s1,
    const oa::fixed_string<N2>& s2) noexcept
    : op_{std::move(op)}, s1_{s1}, s2_{s2}
  {}

  /**
   * Return the binary comparator.
   */
  constexpr auto& op() const noexcept { return op_; }

  /**
   * Return the first fixed string.
   */
  constexpr auto& s1() const noexcept { return s1_; }

  /**
   * Return the second fixed string.
   */
  constexpr auto& s2() const noexcept { return s2_; }

private:
  F op_;
  oa::fixed_string<N1> s1_;
  oa::fixed_string<N2> s2_;
};

// user-defined deduction guide for CTAD
template <typename F, typename T, typename U>
requires (oa::fixed_string_input<T> && oa::fixed_string_input<U>)
fixed_string_binary_test_case(F&&, const T&, const U&) ->
  fixed_string_binary_test_case<
    F, oa::detail::fixed_size_v<T>, oa::detail::fixed_size_v<U>
  >;

/**
 * `fixed_string` equality comparator.
 *
 * This specifically tests the `operator==` overload that takes a
 * `const fixed_string<N1>&` and a `const char (&)[N2])`.
 */
struct fixed_string_equal_1 {
  /**
   * Compare the two `fixed_string` contents for equality using `operator==`.
   *
   * @tparam N1 Length of first string
   * @tparam N2 Length of second string
   *
   * @param s1 First fixed string
   * @param s2 Second fixed string
   */
  template <std::size_t N1, std::size_t N2>
  constexpr bool operator()(
    const oa::fixed_string<N1>& s1,
    const oa::fixed_string<N2> s2) const noexcept
  {
    // copy s2 into char array
    char buf[N2 + 1u];
    for (decltype(N2) i = 0u; i < N2; i++)
      buf[i] = s2[i];
    // null terminate
    buf[N2] = '\0';
    // explicitly select operator== overload
    return s1 == buf;
  }
};

/**
 * `fixed_string` equality operator.
 *
 * This specifically tests the `operator==` overload that takes a
 * `const char (&)[N1]` and a `const fixed_string<N2>&`.
 */
struct fixed_string_equal_2 {
  /**
   * Compare the two `fixed_string` contents for equality using `operator==`.
   *
   * @tparam N1 Length of first string
   * @tparam N2 Length of second string
   *
   * @param s1 First fixed string
   * @param s2 Second fixed string
   */
  template <std::size_t N1, std::size_t N2>
  constexpr bool operator()(
    const oa::fixed_string<N1>& s1,
    const oa::fixed_string<N2>& s2) const noexcept
  {
    // copy s1 into char array
    char buf[N1 + 1u];
    for (decltype(N1) i = 0u; i < N1; i++)
      buf[i] = s1[i];
    // null terminate
    buf[N1] = '\0';
    // explicitly select operator== overload
    return buf == s2;
  }
};

/**
 * `fixed_string` stream equality operator.
 *
 * This first uses `operator<<` to stream both `fixed_string` and then compare
 * the resulting strings from the string streams.
 */
struct fixed_string_sstream_equal {
  /**
   * Compare the two `fixed_string` contents for equality.
   *
   * @tparam N1 Length of first string
   * @tparam N2 Length of second string
   *
   * @param s1 First fixed string
   * @param s2 Second fixed string
   */
  template <std::size_t N1, std::size_t N2>
  bool operator()(
    const oa::fixed_string<N1>& s1,
    const oa::fixed_string<N2>& s2) const noexcept
  {
    // stream both fixed_string instances
    std::stringstream ss1;
    ss1 << s1;
    std::stringstream ss2;
    ss2 << s2;
    // compare using operator== for std::string
    return std::move(ss1).str() == std::move(ss2).str();
  }
};

}  // namespace

// binary_format_traits specializations for fixed_string_equal_(1|2)
namespace oa {
namespace testing {

template <>
struct binary_format_traits<fixed_string_equal_1> {
  static constexpr const char op_string[] = "==";
};

template <>
struct binary_format_traits<fixed_string_equal_2> {
  static constexpr const char op_string[] = "==";
};

}  // namespace testing
}  // namespace oa

namespace {

// create test cases
constexpr auto fixed_string_test_cases = std::make_tuple(
  // 1. test normal concatenation
  fixed_string_binary_test_case{
    std::equal_to{},
    "the quick brown fox jumped",
    oa::fixed_string{"the ", "quick ", "brown ", "fox ", "jumped"}
  },
  // 2. test concatenation with empty string literals
  fixed_string_binary_test_case{
    std::equal_to{},
    "hello world",
    oa::fixed_string{"", "", "", "hello", "" , " ", "", "world", "", "", ""}
  },
  // 3. test operator!= synthesized from operator==
  fixed_string_binary_test_case{std::not_equal_to{}, "first", "second"},
  // 4. test concatenation with fragmented literals
  fixed_string_binary_test_case{
    std::equal_to{},
    "fight until your last breath",
    oa::fixed_string{
      "fi", "ght", " ", "un", "til", " yo", "ur ", "last", " br", "ea", "th"
    }
  },
  // 5. test concatenation with all 3 operator+ overloads
  fixed_string_binary_test_case{
    std::equal_to{},
    "i say hello goodbye",
    "i "
    +  // operator+(const char (&)[N1], const fixed_string<N2>&)
    oa::fixed_string{"say"}
    +  // operator+(const fixed_string<N1>&, const char (&)[N2])
    " hello "
    +  // operator+(const fixed_string<N1>&, const fixed_string<N2>&)
    oa::fixed_string{"goodbye"}
  },
  // 6. test char[] and fixed_string operator==
  fixed_string_binary_test_case{fixed_string_equal_1{}, "hello", "hello"},
  // 7. test fixed_string and char[] operator==
  fixed_string_binary_test_case{fixed_string_equal_2{}, "world", "world"},
  // 8. test comparison after string conversion
  fixed_string_binary_test_case{fixed_string_sstream_equal{}, "jello", "jello"},
  // 9. test char[] and fixed_string operator== (runtime)
  fixed_string_binary_test_case{fixed_string_equal_1{}, "burger", "burger"},
  // 10. test fixed_String and char[] operator== (runtime)
  fixed_string_binary_test_case{fixed_string_equal_2{}, "airplane", "airplane"}
);

// indices of tests that should be evaluated at runtime
constexpr const std::size_t fixed_string_runtime_test_cases[] = {7, 8, 9};

/**
 * `fixed_string` template test fixture.
 *
 * @tparam T `std::index_sequence<I>` to index into `fixed_string_test_cases`
 */
template <typename T>
class FixedStringTest : public ::testing::Test {
private:
  /**
   * Obtain a reference to the specified test case input.
   *
   * @tparam I Index of test case in `fixed_string_test_cases`
   */
  template <std::size_t I>
  static constexpr auto& input(std::index_sequence<I>) noexcept
  {
    static_assert(I < std::tuple_size_v<decltype(fixed_string_test_cases)>);
    return std::get<I>(fixed_string_test_cases);
  }

  /**
   * Indicate if the specified test case should be evaluated at compile-time.
   *
   * @tparam I Index of test case in `fixed_string_test_cases`
   */
  template <std::size_t I>
  static constexpr bool is_compile_time(std::index_sequence<I>) noexcept
  {
    for (decltype(I) i = 0u; i < std::size(fixed_string_runtime_test_cases); i++)
      if (I == fixed_string_runtime_test_cases[i])
        return false;
    return true;
  }

public:
  /**
   * Obtain a reference to the specified test case input.
   */
  static constexpr auto& input() noexcept
  {
    return input(T{});
  }

  /**
   * Indicate if the specified test case should be evaluated at compile-time.
   */
  static constexpr bool is_compile_time() noexcept
  {
    return is_compile_time(T{});
  }

protected:
  /**
   * Perform test and report assertion results.
   *
   * Whether or not the test is evaluated at compile-time or at run-time is
   * based on the return value of `is_compile_time()`.
   */
  void operator()() const
  {
    // reference to comparator, s1, s2
    constexpr auto& op = input().op();
    constexpr auto& s1 = input().s1();
    constexpr auto& s2 = input().s2();
    // if compile-time test
    if constexpr (is_compile_time())
      EXPECT_TRUE(oa::testing::make_result<op(s1, s2)>(op, s1, s2));
    // run-time test
    else
      EXPECT_TRUE(oa::testing::make_result(op, s1, s2));
  }
};

/**
 * Traits type used to construct a `::testing::Types` for `FixedStringTest`.
 *
 * @tparam T `std::index_sequence<Is...>`
 */
template <typename T>
struct fixed_string_input_types_impl {};

/**
 * Traits type used to construct a `::testing::Types` for `FixedStringTest`.
 *
 * @tparam Is Indices 0 through the size of `fixed_string_test_cases` - 1
 */
template <std::size_t... Is>
struct fixed_string_input_types_impl<std::index_sequence<Is...>> {
  using type = ::testing::Types<std::index_sequence<Is>...>;
};

/**
 * Traits type providing the `::testing::Types` for `FixedStringTest`.
 *
 * @tparam N Number of test cases
 */
template <std::size_t N>
using fixed_string_input_types =
  typename fixed_string_input_types_impl<std::make_index_sequence<N>>::type;

TYPED_TEST_SUITE(
  FixedStringTest,
  fixed_string_input_types<std::tuple_size_v<decltype(fixed_string_test_cases)>>
);

/**
 * Evaluate the test case and report the results.
 */
TYPED_TEST(FixedStringTest, Test)
{
  (*this)();
}

}  // namespace
