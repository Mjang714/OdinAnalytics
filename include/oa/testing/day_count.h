/**
 * @file testing/day_count.h
 * @author Derek Huang
 * @brief C++ header for GoogleTest day count testing infrastructure
 * @copyright MIT License
 */

#ifndef OA_TESTING_DAY_COUNT_H_
#define OA_TESTING_DAY_COUNT_H_

#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>

#include <gtest/gtest.h>

#include "oa/common.h"
#include "oa/testing/gtest.h"
#include "oa/time/date.h"

namespace oa {
namespace testing {

/**
 * `DayCounterBase` GoogleTest test case template.
 *
 * This should be used as a concrete base type for a GoogleTest test class
 * template used with `TYPED_TEST()` and instantiated with index types provided
 * by the `oa::testing::index_types<N>` template. For example:
 *
 * @code{.cc}
 * // base template to conveniently manage test data
 * template <typename T = void>
 * class DayCountAct365FixedTest {
 * public:
 *   // convenience type alias for the input I base type
 *   template <std::size_t I>
 *   using base_type = oa::testing::DayCountTestBase<
 *     DayCountAct365FixedTest<oa::testing::index<I>>
 *   >;
 *
 *   // test case inputs
 *   static constexpr auto inputs = std::make_tuple(
 *     // ...
 *   );
 *   // number of test case inputs
 *   static constexpr auto inputs_size = std::tuple_size_v<decltype(inputs)>;
 *
 *   // helper for input I
 *   template <std::size_t I>
 *   static constexpr auto& input() noexcept
 *   {
 *     return std::get<I>(inputs);
 *   }
 * };
 *
 * // partial specialization for test input I
 * template <std::size_t I>
 * class DayCountAct365FixedTest<oa::testing::index<I>>
 *   : public DayCountAct365FixedTest<>::base_type<I> {
 * public:
 *   oa::time::DayCountAct365Fixed dc;
 *   static constexpr auto& input = DayCountAct365FixedTest<>::input<I>();
 * };
 *
 * // test types instantiation
 * TYPED_TEST_SUITE(
 *   DayCountAct365FixedTest,
 *   oa::testing::index_types<DayCountAct365FixedTest<>::inputs_size>
 * );
 *
 * // instantiate test
 * TYPED_TEST(DayCountAct265FixedTest, Test)
 * {
 *   (*this)();
 * }
 * @endcode
 *
 * This allows elimination of repeated `EXPECT_EQ()` and `EXPECT_DOUBLE_EQ()`
 * calls for each input test case and finely separates each individual test
 * case input into a separate test. Note that in this example, we use the base
 * template for `DayCountAct365FixedTest<T>` with `T` defaulted to `void` as a
 * convenient place to hold static members, i.e. the test inputs, the total
 * number of inputs, as well as a helper for the CRTP base and a wrapper to
 * retrieve the appropriate input given the test index `I`.
 *
 * Note that the two required members of the derived test type are:
 *
 *  - `dc`, a `DayCounterBase` derived type object or an indirectly readable
 *    object to a `DayCounterBase`, e.g. a `std::unique_ptr<DayCounterBase>`
 *  - `input`, a copy or const reference to the test input `I`
 *
 * Both `dc` and `input` can be static or non-static members, but it is
 * recommended to at least make `input` static and `constexpr` if possible.
 * Furthermore, the only accepted test case input types are as follows:
 *
 *  - `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, int>`
 *    for a test case testing the day count between two dates
 *  - `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, double>`
 *    for a test case testing the year fraction between two dates
 *
 * @tparam T `TYPED_TEST()` test case template where `oa::testing::index<I>` is
 *  the intended template parameter, `I` indexing into the test cases, although
 *  one can pass `void` or other types for test-specific purposes
 */
template <typename T>
class DayCountTestBase : public ::testing::Test {};

/**
 * Partial specialization for a single test case input `I`.
 *
 * This implements the shared day counter testing logic for test input `I`.
 *
 * @tparam I Test case input index
 * @tparam T `DayCountTestBase` template derived type
 */
template <std::size_t I, template <typename> typename T>
class DayCountTestBase<T<oa::testing::index<I>>> : public ::testing::Test {
private:
  // convenience helpers types for the derived test type + the date type
  using test_type = T<oa::testing::index<I>>;
  using date_type = oa::time::Date;

  /**
   * Return a const reference to `*this` cast to `test_type`.
   *
   * This provides a simpler way to get the derived test type without needing
   * to use a verbose `static_cast` to `const test_type*`.
   */
  constexpr auto& self() const noexcept
  {
    return *static_cast<const test_type*>(this);
  }

  /**
   * Return the day count between two dates.
   *
   * @param d1 First date
   * @param d2 Second date
   */
  auto day_count(const date_type& d1, const date_type& d2) const
  {
    if constexpr (std::indirectly_readable<decltype(self().dc)>)
      return (*self().dc).DayCount(d1, d2);
    else
      return self().dc.DayCount(d1, d2);
  }

  /**
   * Return the year fraction computed between two dates.
   *
   * @param d1 First date
   * @param d2 Second date
   */
  auto year_fraction(const date_type& d1, const date_type& d2) const
  {
    if constexpr (std::indirectly_readable<decltype(self().dc)>)
      return (*self().dc).YearFraction(d1, d2);
    else
      return self().dc.YearFraction(d1, d2);
  }

  /**
   * Dispatcher type for test case `I`.
   *
   * @note `J` is a redundant parameter to enable partial specialization.
   *
   * @tparam J Test case index `I`
   * @tparam T_ Type of test case `I`
   */
  template <std::size_t J, typename T_>
  struct dispatch_type {};

  /**
   * Partial specialization for checking day counts.
   *
   * @tparam J Test case index `I`
   */
  template <std::size_t J>
  struct dispatch_type<
    J,
    std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, int>
  > {
    /**
     * Check that the computed day count between dates is as expected.
     *
     * @param test Test case instance
     */
    void operator()(const test_type& test) const
    {
      // date inputs
      auto [y1, m1, d1] = std::get<0>(test.input);
      auto [y2, m2, d2] = std::get<1>(test.input);
      // expected day count
      auto days = std::get<2>(test.input);
      // create dates + check
      oa::time::Date dt1{y1, m1, d1};
      oa::time::Date dt2{y2, m2, d2};
      EXPECT_EQ(days, test.day_count(dt1, dt2));
    }
  };

  /**
   * Partal specialization for checking year fractions.
   *
   * @tparam J Test case index `I`
   */
  template <std::size_t J>
  struct dispatch_type<
    J,
    std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, double>
  > {
    /**
     * Check that the computed year fraction between dates is as expected.
     *
     * @param test Test case instance
     */
    void operator()(const test_type& test) const
    {
      // date inputs
      auto [y1, m1, d1] = std::get<0>(test.input);
      auto [y2, m2, d2] = std::get<1>(test.input);
      // expected year fraction
      auto years = std::get<2>(test.input);
      // create dates + check
      oa::time::Date dt1{y1, m1, d1};
      oa::time::Date dt2{y2, m2, d2};
      EXPECT_DOUBLE_EQ(years, test.year_fraction(dt1, dt2));
    }
  };

protected:
  /**
   * Invoke the correct test using a dispatcher specialization.
   */
  void operator()() const
  {
    dispatch_type<I, std::decay_t<decltype(self().input)>>{}(self());
  }
};

/**
 * Declare a day count test template with the given name and inputs.
 *
 * This defines the base GoogleTest test class to hold the input value tuple
 * under the assumption that a `constexpr` invocation of `std::make_tuple(...)`
 * is used to create the test case input values.
 *
 * @param test_name GoogleTest test class name
 * @param ... Test inputs of type
 *  `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, int>` or
 *  `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, double>`
 */
#define OA_DECLARE_DAY_COUNT_TEST(test_name, ...) \
  /* base template for managing test inputs and other data */ \
  template <typename T = void> \
  class test_name { \
  public: \
    /* base type template for input I */ \
    template <std::size_t I> \
    using base_type = oa::testing::DayCountTestBase< \
      test_name<oa::testing::index<I>> \
    >; \
    /* test inputs + number of test inputs */ \
    static constexpr auto inputs = std::make_tuple(__VA_ARGS__); \
    static constexpr auto inputs_size = std::tuple_size_v<decltype(inputs)>; \
    /* helper to get a reference to input I */ \
    template <std::size_t I> \
    static constexpr auto& input() noexcept \
    { \
      return std::get<I>(inputs); \
    } \
  }

/**
 * Define the day count test template with the given name and day counter type.
 *
 * This defines the `oa::testing::index<I>` partial specialization for the test
 * clas template, instantiates the types for `TYPED_TEST_SUITE()`, and defines
 * a test named `Test` using `TYPED_TEST()` for the test class template.
 */
#define OA_DEFINE_DAY_COUNT_TEST(test_name, day_count_type) \
  /* partial specialization for test case input I */ \
  template <std::size_t I> \
  class test_name<oa::testing::index<I>> : public test_name<>::base_type<I> { \
  public: \
    /* day counter member + test input I reference */ \
    day_count_type dc; \
    static constexpr auto& input = test_name<>::input<I>(); \
  }; \
  \
  /* instantiate index<I> types */ \
  TYPED_TEST_SUITE( \
    test_name, \
    oa::testing::index_types<test_name<>::inputs_size> \
  ); \
  \
  /* instantiate the test */ \
  TYPED_TEST(test_name, Test) \
  { \
    (*this)(); \
  } \
  /* enforce terminating semicolon */ \
  static_assert(true)

#if 0
// helper macros to get the first and second values of an (a, b) tuple
#define OA_PP_GET_2_0(a, b) a
#define OA_PP_GET_2_1(a, b) b

/**
 * Instantiate the types and `TYPED_TEST()` for a `DayCountTestBase<T>` test.
 *
 * This defines all the necessary templates for a `DayCountTestBase<T>` derived
 * test, using the assumption that a `std::make_tuple(...)` will used to create
 * the test case inputs. If no additional customizations need to be made, this
 * macro simply requires the template type name + a list of test inputs.
 *
 * The test associated with the test type is simply called `Test`.
 *
 * @note This macro cannot be used until GoogleTest changes their typed test
 *  macros to first properly expand their arguments.
 *
 * @param name_and_type `(name, type)` tuple where `name` if the name of the
 *  GoogleTest test class and `type` is the `oa::time::` day counter type
 * @param ... Test inputs of type
 *  `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, int>` or
 *  `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, double>`
 */
#define OA_DAY_COUNT_TEST(name_and_type, ...) \
  /* base template for managing test inputs and other data */ \
  template <typename T = void> \
  class OA_PP_GET_2_0 name_and_type { \
  public: \
    /* test inputs + number of test inputs */ \
    static constexpr auto inputs = std::make_tuple(__VA_ARGS__); \
    static constexpr auto inputs_size = std::tuple_size_v<decltype(inputs)>; \
    /* helper to get a reference to input I */ \
    template <std::size_t I> \
    static constexpr auto& input() noexcept { return std::get<I>(inputs); } \
  }; \
  \
  /* partial specialization for test case input I */ \
  template <std::size_t I> \
  class OA_PP_GET_2_0 name_and_type <oa::testing::index<I>> \
    : public oa::testing::DayCountTestBase< \
        OA_PP_GET_2_0 name_and_type <oa::testing::index<I>> > { \
  public: \
    /* day counter member + test input */ \
    OA_PP_GET_2_1 name_and_type dc; \
    /* reference to input I */ \
    static constexpr auto& input = OA_PP_GET_2_0 name_and_type <>::input<I>(); \
  }; \
  \
  /* instantiate index<I> types */ \
  TYPED_TEST_SUITE( \
    OA_PP_GET_2_0 name_and_type, \
    oa::testing::index_types<OA_PP_GET_2_0 name_and_type <>::inputs_size> \
  ); \
  \
  /* instantiate the test for test_type*/ \
  TYPED_TEST(OA_PP_GET_2_0 name_and_type, Test) \
  { \
    (*this)(); \
  } \
  /* enforce terminating semicolon */ \
  static_assert(true)
#endif  // 0

}  // namespace testing
}  // namespace oa

#endif  // OA_TESTING_DAY_COUNT_H_
