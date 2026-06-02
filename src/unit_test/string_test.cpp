/**
 * @file string_test.cpp
 * @author Derek Huang
 * @brief string.h unit tests
 * @copyright MIT License
 */

#include "oa/string.h"

#include <algorithm>
#include <string>

#include <gtest/gtest.h>

#include "oa/common.h"    // for OA_IDENTITY()

namespace {

/**
 * Test fixture for string tests.
 */
class StringTest : public ::testing::Test {};

/**
 * Test fixture template for compile-time string tests.
 *
 * @tparam T Input test type
 */
template <typename T>
class StringTemplateTest : public StringTest {};

/**
 * Test fixture for `to_upper` tests.
 */
class ToUpperTest : public StringTest {
protected:
  /**
   * Convert the given character to uppercase in-place.
   *
   * @param c Input character
   */
  static void ToUpper(char& c) noexcept { c = oa::to_upper(c); }
};

/**
 * Test that `to_upper()` works as expected with a printable ASCII string.
 */
TEST_F(ToUpperTest, PrintableStringTest)
{
  std::string a{"aHjs78S;ji1ER1nmp?"};
  std::string b{"AHJS78S;JI1ER1NMP?"};
  std::ranges::for_each(a, ToUpper);
  EXPECT_EQ(a, b);
}

/**
 * Test that `to_upper()` works as expected with non-printable characters.
 */
TEST_F(ToUpperTest, NonPrintableStringTest)
{
  // note: the hex characters purposefully have the leading bit set
  std::string a{"\nsdf\r\tdf#O\xa7\x1e\xff"};
  std::string b{"\nSDF\r\tDF#O\xa7\x1e\xff"};
  std::ranges::for_each(a, ToUpper);
  EXPECT_EQ(a, b);
}

/**
 * Test fixture for `to_lower` tests.
 */
class ToLowerTest : public StringTest {
protected:
  /**
   * Convert the given character to lowercase in-place.
   *
   * @param c Input character
   */
  static void ToLower(char& c) noexcept { c = oa::to_lower(c); }
};

/**
 * Test that `to_lower()` works as expected with a printable ASCII string.
 */
TEST_F(ToLowerTest, PrintableStringTest)
{
  std::string a{"Ihh7t8GAG9UIHJo;as"};
  std::string b{"ihh7t8gag9uihjo;as"};
  std::ranges::for_each(a, ToLower);
  EXPECT_EQ(a, b);
}

/**
 * Test that `to_lower()` works as expected with non-printable characters.
 */
TEST_F(ToLowerTest, NonPrintableStringTest)
{
  std::string a{"\xde\x21UIhj98\r\t\tA8U\n\x9a"};
  std::string b{"\xde\x21uihj98\r\t\ta8u\n\x9a"};
  std::ranges::for_each(a, ToLower);
  EXPECT_EQ(a, b);
}

/**
 * Test fixture for `is_digit` tests.
 */
class IsDigitTest : public StringTest {
protected:
  /**
   * Convert the given character to `1` if it is a digit else `0` in-place.
   *
   * @param c Input character
   */
  static void IsDigit(char& c) noexcept { c = (oa::is_digit(c)) ? '1' : '0'; }
};

/**
 * Test that `is_digit()` works as expected with a printable ASCII string.
 */
TEST_F(IsDigitTest, PrintableStringTest)
{
  std::string a{"uhyH7Y9898YUu0U4K8OU9012"};
  std::string b{"000010111100010101001111"};
  std::ranges::for_each(a, IsDigit);
  EXPECT_EQ(a, b);
}

/**
 * Test that `is_digit()` works as expected with non-printable characters.
 */
TEST_F(IsDigitTest, NonPrintableStringTest)
{
  std::string a{"sdf8y29y38\n45\t9u1\x1e\x3e\n\n09"};
  std::string b{"00010110110110101000011"};
  std::ranges::for_each(a, IsDigit);
  EXPECT_EQ(a, b);
}

/**
 * `to_upper()` compile-time character conversion test case input.
 *
 * @tparam a Input character
 * @tparam b Expected output character
 */
template <char a, char b>
struct to_upper_test_case {};

/**
 * Partial specialization for `to_upper_test_case<a, b>`.
 *
 * @tparam a Input character
 * @tparam b Expected output character
 */
template <char a, char b>
class StringTemplateTest<to_upper_test_case<a, b>> : public ToUpperTest {
protected:
  /**
   * Invoke `to_upper(a)` at compile time and report the result.
   */
  void operator()() const
  {
    constexpr char c = oa::to_upper(a);
    EXPECT_EQ(b, c);
  }
};

/**
 * `to_lower()` compile-time character conversion test case input.
 *
 * @tparam a Input character
 * @tparam b Expected output character
 */
template <char a, char b>
struct to_lower_test_case {};

/**
 * Partial specialization for `to_lower_test_case<a, b>`.
 *
 * @tparam a Input character
 * @tparam b Expected output character
 */
template <char a, char b>
class StringTemplateTest<to_lower_test_case<a, b>> : public ToLowerTest {
protected:
  /**
   * Invoke `to_lower(a)` at compile time and report the result.
   */
  void operator()() const
  {
    constexpr char c = oa::to_lower(a);
    EXPECT_EQ(b, c);
  }
};

/**
 * `is_digit()` compile-time test case input.
 *
 * @tparam a Input character
 * @tparam b Expected result
 */
template <char a, bool b>
struct is_digit_test_case {};

/**
 * Partial specialization for `is_digit_test_case<a, b>`.
 *
 * @tparam a Input character
 * @tparam b Expected result
 */
template <char a, bool b>
class StringTemplateTest<is_digit_test_case<a, b>> : public IsDigitTest {
protected:
  /**
   * Invoke `is_digit(a)` at compile time and report the result.
   */
  void operator()() const
  {
    constexpr bool c = oa::is_digit(a);
    EXPECT_EQ(b, c);
  }
};

// instantiate StringTemplateTest
TYPED_TEST_SUITE(
  StringTemplateTest,
  OA_IDENTITY(
    ::testing::Types<
      // to_upper() test cases
      to_upper_test_case<'a', 'A'>,
      to_upper_test_case<'$', '$'>,
      to_upper_test_case<'H', 'H'>,
      to_upper_test_case<'\n', '\n'>,
      to_upper_test_case<'\xfa', '\xfa'>,
      // to_lower() test cases
      to_lower_test_case<'B', 'b'>,
      to_lower_test_case<'?', '?'>,
      to_lower_test_case<'c', 'c'>,
      to_lower_test_case<'\x1e', '\x1e'>,
      to_lower_test_case<'\t', '\t'>,
      // is_digit() test cases
      is_digit_test_case<'a', false>,
      is_digit_test_case<'4', true>,
      is_digit_test_case<'\n', false>,
      is_digit_test_case<'\x9a', false>,
      is_digit_test_case<'7', true>,
      is_digit_test_case<'a' + '6' - 'a', true>
    >
  )
);

/**
 * Invoke the `StringTemplateTest<T>` test.
 */
TYPED_TEST(StringTemplateTest, Test)
{
  (*this)();
}

}  // namespace
