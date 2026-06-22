/**
 * @file oper12_variant_test.cpp
 * @author Derek Huang
 * @brief oper12.h tests with `std::variant` and `oper12::as<T>()`
 * @copyright MIT License
 */

#include "oa/accel/oper12.h"

#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "oa/accel/enums.h"
#include "oa/accel/xl_conv.h"  // for multi_conv_options

namespace {

/**
 * Helper type that contains a type pack.
 *
 * @tparam Ts types
 */
template <typename... Ts>
struct type_wrapper {};

/**
 * Traits to obtain a `::testing::Types<Ts...>` from the given types.
 *
 * @tparam Ts types
 */
template <typename... Ts>
struct gtest_types_from {
  using type = ::testing::Types<Ts...>;
};

/**
 * Partial specialization for `std::variant<Ts...>`.
 *
 * This passes the variant and each type using a `type_wrapper<T, U>`.
 *
 * @tparam Ts types
 */
template <typename... Ts>
struct gtest_types_from<std::variant<Ts...>> {
  using type = ::testing::Types<type_wrapper<Ts, std::variant<Ts...>>...>;
};

/**
 * Base typed test fixture for `oper12` variant tests.
 *
 * Each partial specialization should be defined using `OPER12_VARIANT_TEST()`.
 */
template <typename T>
class Oper12VariantTest : public ::testing::Test {};

/**
 * Macro for declaring an appropriate `Oper12VariantTest<T>` specialization.
 *
 * This should be used in the following manner:
 *
 * @code{.cc}
 * OPER12_VARIANT_TEST(Ts, input_type) {
 * protected:
 *   void operator()() const
 *   {
 *     // ...
 *   }
 * };
 * @endcode
 *
 * @note The `std::variant<Ts...>` is the second `type_wrapper<Ts...>` argument
 *  so it is easier to determine from CTest output what type is actually being
 *  used in the test case (test name gets way too long).
 *
 * @param Ts Variant type pack
 * @param type Input type
 */
#define OPER12_VARIANT_TEST(Ts, type) \
  template <typename... Ts> \
  class Oper12VariantTest<type_wrapper<type, std::variant<Ts...>>> \
    : public ::testing::Test

/**
 * `std::monostate` specialization.
 */
OPER12_VARIANT_TEST(Ts, std::monostate) {
protected:
  /**
   * Check that `from()` works with `std::monostate`.
   */
  void operator()() const
  {
    std::variant<Ts...> v;
    auto op = oa::accel::oper12::from(v);
    // std::monostate corresponds to nil xloper12
    EXPECT_EQ(oa::accel::xltype::nil, op.type());
  }
};

/**
 * `bool` specialization.
 */
OPER12_VARIANT_TEST(Ts, bool) {
protected:
  /**
   * Check that `from()` works with `bool`.
   */
  void operator()() const
  {
    constexpr bool value = true;
    std::variant<Ts...> v{value};
    auto op = oa::accel::oper12::from(v);
    EXPECT_EQ(value, op.as<bool>(oa::accel::strict));
  }
};

/**
 * `int` specialization.
 */
OPER12_VARIANT_TEST(Ts, int) {
protected:
  /**
   * Check that `from()` works with `int`.
   */
  void operator()() const
  {
    constexpr int value = 123;
    std::variant<Ts...> v{value};
    auto op = oa::accel::oper12::from(v);
    EXPECT_EQ(value, op.as<int>(oa::accel::strict));
  }
};

/**
 * `const char*` specialization.
 */
OPER12_VARIANT_TEST(Ts, const char*) {
protected:
  /**
   * Check that `from()` works with `const char*`.
   */
  void operator()() const
  {
    constexpr auto str = "lorem ipsum dolorem est";
    std::variant<Ts...> v{str};
    auto op = oa::accel::oper12::from(v);
    EXPECT_EQ(str, op.as<std::string>(oa::accel::strict));
  }
};

/**
 * `std::string` specialization.
 */
OPER12_VARIANT_TEST(Ts, std::string) {
protected:
  /**
   * Check that `from()` works with `std::string`.
   */
  void operator()() const
  {
    std::string str{"hello world"};
    std::variant<Ts...> v{str};
    auto op = oa::accel::oper12::from(v);
    EXPECT_EQ(str, op.as<std::string>(oa::accel::strict));
  }
};

/**
 * `std::string_view` specialization.
 */
OPER12_VARIANT_TEST(Ts, std::string_view) {
protected:
  /**
   * Check that `from()` works with `std::string_view`.
   *
   * There is no `as<std::string_view>()` so we still convert back to string.
   */
  void operator()() const
  {
    std::string_view str{"quick fox lazy dog"};
    std::variant<Ts...> v{str};
    auto op = oa::accel::oper12::from(v);
    // note: no operator== for std::string and std::string_view
    EXPECT_EQ(std::string{str}, op.as<std::string>(oa::accel::strict));
  }
};

/**
 * `std::wstring` specialization.
 */
OPER12_VARIANT_TEST(Ts, std::wstring) {
protected:
  /**
   * Check that `from()` works with `std::wstring`.
   */
  void operator()() const
  {
    std::wstring str{L"hello long world"};
    std::variant<Ts...> v{str};
    auto op = oa::accel::oper12::from(v);
    EXPECT_EQ(str, op.as<std::wstring>(oa::accel::strict));
  }
};

/**
 * `std::wstring_view` specialization.
 */
OPER12_VARIANT_TEST(Ts, std::wstring_view) {
protected:
  /**
   * Check that `from()` works with `std::wstring_view`.
   *
   * The `as<std::wstring_view>()` directly accesses the `xloper12` value.
   */
  void operator()() const
  {
    std::wstring_view str{L"quick fox long lazy dog"};
    std::variant<Ts...> v{str};
    auto op = oa::accel::oper12::from(v);
    EXPECT_EQ(str, op.as<std::wstring_view>());  // naturally strict
  }
};

/**
 * `std::vector<float>` specialization.
 */
OPER12_VARIANT_TEST(Ts, std::vector<float>) {
protected:
  /**
   * Check that `from()` works with `std::vector<float>`.
   *
   * The incoming vector is treated as a column vector automatically.
   */
  void operator()() const
  {
    std::vector<float> vec{1.f, 2.f, 3.f, 4.f};
    std::variant<Ts...> v{vec};
    auto op = oa::accel::oper12::from(v);
    // check dimensions
    EXPECT_EQ(vec.size(), op.rows());
    EXPECT_EQ(1u, op.cols());
    // check values
    auto res = op.as<decltype(vec)>();
    EXPECT_THAT(res, ::testing::Pointwise(::testing::FloatEq(), vec));
  }
};

/**
 * `std::vector<double>` specialization.
 */
OPER12_VARIANT_TEST(Ts, std::vector<double>) {
protected:
  /**
   * Check that `from()` works with `std::vector<double>`.
   *
   * The incoming vector is treated as a column vector automatically.
   */
  void operator()() const
  {
    std::vector<double> vec{2., 4., 6., 8.};
    std::variant<Ts...> v{vec};
    auto op = oa::accel::oper12::from(v);
    // check dimensions
    EXPECT_EQ(vec.size(), op.rows());
    EXPECT_EQ(1u, op.cols());
    // check values
    auto res = op.as<decltype(vec)>();
    EXPECT_THAT(res, ::testing::Pointwise(::testing::DoubleEq(), vec));
  }
};

// ensure we don't pollute unity build translation unit
#undef OPER12_VARIANT_TEST

// variant type used by the Oper12VariantTest
using oper12_variant_test_type = std::variant<
  std::monostate,
  bool,
  int,
  const char*,
  std::string,
  std::string_view,
  std::wstring,
  std::wstring_view,
  std::vector<float>,
  std::vector<double>
>;

TYPED_TEST_SUITE(
  Oper12VariantTest,
  typename gtest_types_from<oper12_variant_test_type>::type
);

/**
 * Execute the `Oper12VariantTest` specialization's test function.
 */
TYPED_TEST(Oper12VariantTest, Test)
{
  (*this)();
}

}  // namespace
