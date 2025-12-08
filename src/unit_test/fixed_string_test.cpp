/**
 * @file fixed_string_test.cpp
 * @author Derek Huang
 * @brief fixed_string.h unit tests
 * @copyright MIT License
 */

#include "oa/fixed_string.h"

#include <sstream>

#include <gtest/gtest.h>

#include "oa/common.h"
#include "oa/internal/unity.h"

namespace {
namespace OA_UNITY_ID {

// test normal concatenation
struct concat_test_1 {
  static constexpr const char expected[] = "the quick brown fox jumped";
  static constexpr oa::fixed_string actual{
    "the ", "quick ", "brown ", "fox ", "jumped"
  };
};

// test concatenation with empty string literals
struct concat_test_2 {
  static constexpr const char expected[] = "hello world";
  static constexpr oa::fixed_string actual{
    "", "", "", "hello", "" , " ", "", "world", "", "", ""
  };
};

// test concatenation with fragmented literals
struct concat_test_3 {
  static constexpr const char expected[] = "fight until your last breath";
  static constexpr oa::fixed_string actual{
    "fi", "ght", " ", "un", "til", " yo", "ur ", "last", " br", "ea", "th"
  };
};

// test concatenation with all 3 operator+ overloads
struct concat_test_4 {
  static constexpr const char expected[] = "i say hello goodbye";
  static constexpr auto actual =
    "i "
    +  // operator+(const char (&)[N1], const fixed_string<N2>&)
    oa::fixed_string{"say"}
    +  // operator+(const fixed_string<N1>&, const char (&)[N2])
    " hello "
    +  // operator+(const fixed_string<N1>&, const fixed_string<N2>&)
    oa::fixed_string{"goodbye"};
};

/**
 * `fixed_string` template test fixture.
 *
 * @tparam T `concat_test_*` test input type
 */
template <typename T>
class FixedStringTest : public ::testing::Test {
protected:
  /**
   * Emit message if expected and actual values are not equal.
   *
   * @note The result is a template parameter to force compile-time evaluation.
   *
   * @tparam res `true` if equal, `false` otherwise
   */
  template <bool res>
  void check() const
  {
    EXPECT_TRUE(res) << "expected != actual [\"" << T::expected <<
      "\" != \"" << T::actual;
  }
};

using FixedStringTestTypes = ::testing::Types<
  concat_test_1, concat_test_2, concat_test_3, concat_test_4
>;
TYPED_TEST_SUITE(FixedStringTest, FixedStringTestTypes);

/**
 * Test `operator==(const fixed_string<N1>&, const char (&)[N2])`.
 */
TYPED_TEST(FixedStringTest, EqTest1)
{
  this->template check<TypeParam::expected == TypeParam::actual>();
}

/**
 * Test `operator==(const char (&)[N1], const fixed_string<N2>&)`.
 */
TYPED_TEST(FixedStringTest, EqTest2)
{
  this->template check<TypeParam::actual == TypeParam::expected>();
}

/**
 * Test `operator==(const fixed_string<N1>&, const fixed_string<N2>&)`.
 */
TYPED_TEST(FixedStringTest, EqTest3)
{
  using T = TypeParam;
  this->template check<oa::fixed_string{T::expected} == T::actual>();
}

/**
 * Test writing the `fixed_string` to an output stream.
 */
TYPED_TEST(FixedStringTest, StreamTest)
{
  std::stringstream ss;
  ss << TypeParam::actual;
  EXPECT_EQ(TypeParam::expected, std::move(ss).str());
}

}  // namespace OA_UNITY_ID
}  // namespace
