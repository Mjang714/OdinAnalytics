/**
 * @file date_test.cpp
 * @author Michael Jang, Derek Huang
 * @brief date.h unit tests
 * @copyright MIT License
 */

#include "oa/time/date.h"

#include <chrono>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "oa/ctti.h"           // for OA_PRETTY_FUNCTION_NAME
#include "oa/testing/gtest.h"  // for index_types, binary_format_traits
#include "oa/time/tenor.h"     // for oa::time::Tenor

namespace {

/**
 * Date `IsLeap()` test case dispatching and input type.
 *
 * This indicates whether the date is expected to be in a leap year or not.
 */
class DateIsLeap {
public:
	constexpr DateIsLeap(bool v) noexcept : v_{v} {}
	constexpr bool value() const noexcept { return v_; }
	constexpr explicit operator bool() const noexcept { return v_; }
private:
	bool v_;
};

/**
 * `Date::DaysInMonth()` test case dispatching and input type.
 *
 * This provides the `Date::DaysInMonth()` input parameters.
 */
class DaysInMonthInput {
public:
	constexpr DaysInMonthInput(int y, int m) noexcept : year_{y}, month_{m} {}
	constexpr int year() const noexcept { return year_; }
	constexpr int month() const noexcept { return month_; }
private:
	int year_;
	int month_;
};

// DateTest inputs
constexpr auto date_template_test_inputs = std::make_tuple(
	// original bad inputs
	std::pair{"190O-12-04", false},
	std::pair{"19-10-2004", false},
	std::pair{"12-10-2007", false},
	std::pair{"12-00-2007", false},
	std::pair{"2007-12-32", false},
	std::pair{"2008-2-30", false},
	std::pair{std::tuple{2008, 2, 30}, false},
	std::pair{std::tuple{2008, 13, 30}, false},
	// additional bad inputs
	std::pair{"bad string", false},
	std::pair{"2000", false},
	std::pair{"2004-", false},
	std::pair{"2005-6", false},
	std::pair{"20-6-6", false},
	std::pair{"2006/09/", false},
	std::pair{"/234sdfs", false},
	std::pair{std::tuple{2005, -12, 111}, false},
	std::pair{std::tuple{-100, 2, 45}, false},
	// test delimiter consistency
	std::pair{"2020/09-24", false},
	std::pair{"2025:1-23", false},
	// original valid inputs
	std::pair{"1900-12-04", true},
	std::pair{"1900:12:04", true},
	std::pair{"1900/12/04", true},
	std::pair{"2007-12-31", true},
	std::pair{"2008-2-28", true},
	std::pair{std::tuple{2008, 2, 28}, true},
	std::pair{std::tuple{2007, 12, 31}, true},
	// test Julian day number and day of week
	std::tuple{"2022-7-31", 2459792, 6},
	std::tuple{"2000-1-1", 2451545, 5},
	// test day of week + string formatting given Julian day number
	std::tuple{2459790, "2022-7-29 : Julian Integer = 2459790", 4},
	std::tuple{2451556, "2000-1-12 : Julian Integer = 2451556", 2},
	// test date creation from std::chrono::system_clock::time_point
	std::pair{std::tuple{1970, 1, 1}, std::chrono::days{0}},
	std::pair{std::tuple{1970, 1, 3}, std::chrono::days{2}},
	std::pair{std::tuple{1970, 1, 12}, std::chrono::days{11}},
	// test if date is in a leap year
	std::pair{std::tuple{2000, 7, 31}, DateIsLeap{true}},
	std::pair{std::tuple{2024, 7, 31}, DateIsLeap{true}},
	std::pair{std::tuple{2100, 1, 1}, DateIsLeap{false}},
	// test if Date::DaysInMonth() works as expected
	std::pair{29, DaysInMonthInput{2000, 2}},
	std::pair{28, DaysInMonthInput{2100, 2}},
	std::pair{28, DaysInMonthInput{2030, 2}},
	std::pair{30, DaysInMonthInput{2010, 11}},
	std::pair{31, DaysInMonthInput{2020, 12}},
	// original binary comparison operation tests
	std::tuple{
		std::tuple{2000, 7, 31},
		std::equal_to{},
		std::tuple{2010, 10, 25},
		false
	},
	std::tuple{
		std::tuple{2000, 7, 31},
		std::greater{},
		std::tuple{2010, 10, 25},
		false
	},
	std::tuple{
		std::tuple{2000, 7, 31},
		std::greater_equal{},
		std::tuple{2010, 10, 25},
		false
	},
	std::tuple{
		std::tuple{2000, 7, 31},
		std::greater{},
		std::tuple{2000, 7, 31},
		false
	},
	std::tuple{
		std::tuple{2000, 7, 31},
		std::less{},
		std::tuple{2000, 7, 31},
		false
	},
	std::tuple{
		std::tuple{2000, 7, 31},
		std::equal_to{},
		std::tuple{2000, 7, 31},
		true
	},
	std::tuple{
		std::tuple{2000, 7, 31},
		std::less_equal{},
		std::tuple{2010, 10, 25},
		true
	},
	std::tuple{
		std::tuple{2000, 7, 31},
		std::less{},
		std::tuple{2010, 10, 25},
		true
	},
	// original AddTenor() tests
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"2D",
		std::equal_to{},
		"2022-7-31 : Julian Integer = 2459792"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"580d",
		std::equal_to{},
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"0d",
		std::equal_to{},
		"2022-7-29 : Julian Integer = 2459790"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::plus{},
		"2w",
		std::equal_to{},
		"2022-8-11 : Julian Integer = 2459803"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::plus{},
		"83w",
		std::equal_to{},
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::plus{},
		"0w",
		std::equal_to{},
		"2022-7-28 : Julian Integer = 2459789"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"2M",
		std::equal_to{},
		"2022-9-29 : Julian Integer = 2459852"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"19M",
		std::equal_to{},
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"7M",
		std::equal_to{},
		"2023-2-28 : Julian Integer = 2460004"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"67M",
		std::equal_to{},
		"2028-2-29 : Julian Integer = 2461831"
	},
	// note: tests adding month to end of a month with 31st as the last day
	std::tuple{
		std::tuple{2022, 8, 31},
		std::plus{},
		"1M",
		std::equal_to{},
		"2022-9-30 : Julian Integer = 2459853"
	},
	std::tuple{
		std::tuple{2024, 2, 29},
		std::plus{},
		"1Y",
		std::equal_to{},
		"2025-2-28 : Julian Integer = 2460735"
	},
	std::tuple{
		std::tuple{2021, 9, 15},
		std::plus{},
		"60Y",
		std::equal_to{},
		"2081-9-15 : Julian Integer = 2481388"
	},
	std::tuple{
		std::tuple{2015, 8, 31},
		std::plus{},
		"100Y",
		std::equal_to{},
		"2115-8-31 : Julian Integer = 2493790"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"-2D",
		std::equal_to{},
		"2022-7-27 : Julian Integer = 2459788"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"-580d",
		std::equal_to{},
		"2020-12-26 : Julian Integer = 2459210"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"-0d",
		std::equal_to{},
		"2022-7-29 : Julian Integer = 2459790"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::plus{},
		"-2w",
		std::equal_to{},
		"2022-7-14 : Julian Integer = 2459775"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::plus{},
		"-83w",
		std::equal_to{},
		"2020-12-24 : Julian Integer = 2459208"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::plus{},
		"-0w",
		std::equal_to{},
		"2022-7-28 : Julian Integer = 2459789"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"-2M",
		std::equal_to{},
		"2022-5-29 : Julian Integer = 2459729"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"-29M",
		std::equal_to{},
		"2020-2-29 : Julian Integer = 2458909"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"-7M",
		std::equal_to{},
		"2021-12-29 : Julian Integer = 2459578"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		"-67M",
		std::equal_to{},
		"2016-12-29 : Julian Integer = 2457752"
	},
	// note: tests subtracting month to end of month with 31st as last day
	std::tuple{
		std::tuple{2022, 8, 31},
		std::plus{},
		"-2M",
		std::equal_to{},
		"2022-6-30 : Julian Integer = 2459761"
	},
	// note: tests subtracting month to end of month with 31st as last day
	std::tuple{
		std::tuple{2022, 8, 31},
		std::plus{},
		"-36M",
		std::equal_to{},
		"2019-8-31 : Julian Integer = 2458727"
	},
	std::tuple{
		std::tuple{2024, 2, 29},
		std::plus{},
		"-1Y",
		std::equal_to{},
		"2023-2-28 : Julian Integer = 2460004"
	},
	std::tuple{
		std::tuple{2081, 9, 15},
		std::plus{},
		"-60Y",
		std::equal_to{},
		"2021-9-15 : Julian Integer = 2459473"
	},
	std::tuple{
		std::tuple{2015, 8, 31},
		std::plus{},
		"-100Y",
		std::equal_to{},
		"1915-8-31 : Julian Integer = 2420741"
	},
	// test adding tenors to dates or dates to tenors
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		std::pair{2, oa::time::Tenors::kDays},
		std::equal_to{},
		std::tuple{2022, 7, 31}
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		std::pair{580, oa::time::Tenors::kDays},
		std::equal_to{},
		std::tuple{2024, 2, 29}
	},
	std::tuple{
		std::pair{0, oa::time::Tenors::kDays},
		std::plus{},
		std::tuple{2022, 7, 29},
		std::equal_to{},
		std::tuple{2022, 7, 29}
	},
	std::tuple{
		std::tuple{2024, 2, 29},
		std::plus{},
		std::pair{1, oa::time::Tenors::kYears},
		std::equal_to{},
		std::tuple{2025, 2, 28}
	},
	std::tuple{
		std::pair{60, oa::time::Tenors::kYears},
		std::plus{},
		std::tuple{2021, 9, 15},
		std::equal_to{},
		std::tuple{2081, 9, 15}
	},
	std::tuple{
		std::tuple{2015, 8, 31},
		std::plus{},
		std::pair{100, oa::time::Tenors::kYears},
		std::equal_to{},
		std::tuple{2115, 8, 31}
	},
	// test adding day counts to dates
	std::tuple{
		std::tuple{2022, 7, 29},
		std::plus{},
		2,
		std::equal_to{},
		std::tuple{2022, 7, 31}
	},
	std::tuple{
		580,
		std::plus{},
		std::tuple{2022, 7, 29},
		std::equal_to{},
		std::tuple{2024, 2, 29}
	},
	std::tuple{
		0,
		std::plus{},
		std::tuple{2022, 7, 29},
		std::equal_to{},
		std::tuple{2022, 7, 29}
	},
	// original SubTenor tests
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"2D",
		std::equal_to{},
		"2022-7-27 : Julian Integer = 2459788"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"580d",
		std::equal_to{},
		"2020-12-26 : Julian Integer = 2459210"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"0d",
		std::equal_to{},
		"2022-7-29 : Julian Integer = 2459790"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::minus{},
		"2w",
		std::equal_to{},
		"2022-7-14 : Julian Integer = 2459775"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::minus{},
		"83w",
		std::equal_to{},
		"2020-12-24 : Julian Integer = 2459208"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::minus{},
		"0w",
		std::equal_to{},
		"2022-7-28 : Julian Integer = 2459789"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"2M",
		std::equal_to{},
		"2022-5-29 : Julian Integer = 2459729"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"29M",
		std::equal_to{},
		"2020-2-29 : Julian Integer = 2458909"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"7M",
		std::equal_to{},
		"2021-12-29 : Julian Integer = 2459578"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"67M",
		std::equal_to{},
		"2016-12-29 : Julian Integer = 2457752"
	},
	// note: tests subtracting month to end of month with 31st as last day
	std::tuple{
		std::tuple{2022, 8, 31},
		std::minus{},
		"2M",
		std::equal_to{},
		"2022-6-30 : Julian Integer = 2459761"
	},
	// note: tests subtracting month to end of month with 31st as last day
	std::tuple{
		std::tuple{2022, 8, 31},
		std::minus{},
		"36M",
		std::equal_to{},
		"2019-8-31 : Julian Integer = 2458727"
	},
	std::tuple{
		std::tuple{2024, 2, 29},
		std::minus{},
		"1Y",
		std::equal_to{},
		"2023-2-28 : Julian Integer = 2460004"
	},
	std::tuple{
		std::tuple{2081, 9, 15},
		std::minus{},
		"60Y",
		std::equal_to{},
		"2021-9-15 : Julian Integer = 2459473"
	},
	std::tuple{
		std::tuple{2015, 8, 31},
		std::minus{},
		"100Y",
		std::equal_to{},
		"1915-8-31 : Julian Integer = 2420741"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"-2D",
		std::equal_to{},
		"2022-7-31 : Julian Integer = 2459792"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"-580d",
		std::equal_to{},
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"0d",
		std::equal_to{},
		"2022-7-29 : Julian Integer = 2459790"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::minus{},
		"-2w",
		std::equal_to{},
		"2022-8-11 : Julian Integer = 2459803"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::minus{},
		"-83w",
		std::equal_to{},
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		std::minus{},
		"-0w",
		std::equal_to{},
		"2022-7-28 : Julian Integer = 2459789"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"-2M",
		std::equal_to{},
		"2022-9-29 : Julian Integer = 2459852"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"-19M",
		std::equal_to{},
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"-7M",
		std::equal_to{},
		"2023-2-28 : Julian Integer = 2460004"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		std::minus{},
		"-67M",
		std::equal_to{},
		"2028-2-29 : Julian Integer = 2461831"
	},
	// note: tests adding month to end of month with 31st as last day
	std::tuple{
		std::tuple{2022, 8, 31},
		std::minus{},
		"-1M",
		std::equal_to{},
		"2022-9-30 : Julian Integer = 2459853"
	},
	std::tuple{
		std::tuple{2024, 2, 29},
		std::minus{},
		"-1Y",
		std::equal_to{},
		"2025-2-28 : Julian Integer = 2460735"
	},
	std::tuple{
		std::tuple{2021, 9, 15},
		std::minus{},
		"-60Y",
		std::equal_to{},
		"2081-9-15 : Julian Integer = 2481388"
	},
	std::tuple{
		std::tuple{2015, 8, 31},
		std::minus{},
		"-100Y",
		std::equal_to{},
		"2115-8-31 : Julian Integer = 2493790"
	}
);

/**
 * `Date` class generic template tests.
 *
 * @tparam T `oa::testing::index<I>` where `I` indexes into the inputs array
 */
template <typename T = void>
class DateTest : public ::testing::Test {
public:
	// size of the test input
	static constexpr auto inputs_size =
		std::tuple_size_v<decltype(date_template_test_inputs)>;
};

/**
 * Partial specialization for the actual `DateTest` instantiation.
 *
 * To handle a new kind of testing input, add a new [partial] specialization
 * for `dispatch_type<J, T>` and add the corresponding `T` inputs to the list
 * of test inputs in the `date_test_input_cases` tuple.
 *
 * @tparam I Input index
 */
template <std::size_t I>
class DateTest<oa::testing::index<I>> : public ::testing::Test {
private:
	// type of the input tuple
	using inputs_type = decltype(date_template_test_inputs);

	// safety check
	static_assert(I < std::tuple_size_v<inputs_type>, "I out of bounds");

	/**
	 * Test dispatcher type.
	 *
	 * This allows us to segregate logic based on the test case input type
	 * while also ensuring we can obtain the input as a constant expression.
	 * Each dispatcher should implement `operator()` to run the testing logic.
	 *
	 * @note The extra index argument enables us to create partial
	 *  specializations by changing `T` as we cannot have explicit template
	 *  specializations in a non-namespace context in C++.
	 *
	 * @tparam J Input index
	 * @tparam T Input type
	 */
	template <std::size_t J, typename T>
	struct dispatch_type {};

	/**
	 * Partial specialization for `std::pair<const char*, bool>`.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<J, std::pair<const char*, bool>> {
		/**
		 * Check that the given input throws or does not throw appropriately.
		 */
		void operator()() const
		{
			// note: works as date_template_test_inputs is constexpr
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// check
			if constexpr (inputs.second)
				EXPECT_NO_THROW(oa::time::Date{inputs.first});
			else
				EXPECT_THROW(oa::time::Date{inputs.first}, std::runtime_error);
		}
	};

	/**
	 * Partial specialization for `std::pair<std::tuple<int, int, int>, bool>`.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<J, std::pair<std::tuple<int, int, int>, bool>> {
		/**
		 * Check that the given input throws or does not throw appropriately.
		 */
		void operator()() const
		{
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// get year, month, day
			auto [y, m, d] = inputs.first;
			// check
			// note: extra parentheses required to treat braced-list-init in
			// macro invocation as a single argument
			if constexpr (inputs.second)
				EXPECT_NO_THROW((oa::time::Date{y, m, d}));
			else
				EXPECT_THROW((oa::time::Date{y, m, d}), std::runtime_error);
		}
	};

	/**
	 * Partial specialization for `std::tuple<const char*, int, int>`.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<J, std::tuple<const char*, int, int>> {
		/**
		 * Check that the given input has the right Julian day and day of week.
		 */
		void operator()() const
		{
			// split into input, Julian day number, and day of week number
			auto [str, jdn, dow] = std::get<J>(date_template_test_inputs);
			// construct + check
			oa::time::Date date{str};
			EXPECT_EQ(jdn, date.GetJulian());
			EXPECT_EQ(dow, date.GetDOWInt());
		}
	};

	/**
	 * Partial specialization for `Date` leap year checking.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<J, std::pair<std::tuple<int, int, int>, DateIsLeap>> {
		/**
		 * Check if the `Date` is in a leap year or not as expected.
		 */
		void operator()() const
		{
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// get date inputs + create date
			auto [y, m, d] = inputs.first;
			oa::time::Date date{y, m, d};
			// check
			EXPECT_EQ(!!inputs.second, date.IsLeap());
		}
	};

	/**
	 * Partial specialization for `DaysInMonth()` tests.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<J, std::pair<int, DaysInMonthInput>> {
		/**
		 * Check if `DaysInMonth()` works as expected.
		 */
		void operator()() const
		{
			auto [month_days, input] = std::get<J>(date_template_test_inputs);
			EXPECT_EQ(
				month_days,
				oa::time::Date::DaysInMonth(input.year(), input.month())
			);
		}
	};

	/**
	 * Traits helper to indicate supported `Date` binary comparison operations.
	 *
	 * @tparam F Binary invocable
	 */
	template <typename F>
	static constexpr bool is_date_comparison_op_v = (
		std::is_same_v<F, std::equal_to<>> ||
		std::is_same_v<F, std::not_equal_to<>> ||
		std::is_same_v<F, std::less<>> ||
		std::is_same_v<F, std::less_equal<>> ||
		std::is_same_v<F, std::greater<>> ||
		std::is_same_v<F, std::greater_equal<>>
	);

	/**
	 * Partial specialization for `Date` binary comparisons.
	 *
	 * @tparam J Input index
	 * @tparam C Binary comparison operator
	 */
	template <std::size_t J, typename C>
	requires (is_date_comparison_op_v<C>)
	struct dispatch_type<
		J,
		std::tuple<
			std::tuple<int, int, int>,  // date 1
			C,                          // ==, !=, <, <=, >, >=
			std::tuple<int, int, int>,  // date 2
			bool                        // expected comparison result
		>
	> {
		/**
		 * Check that two `Date` objects compare as expected.
		 */
		void operator()() const
		{
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// get date inputs
			auto [y1, m1, d1] = std::get<0>(inputs);
			auto [y2, m2, d2] = std::get<2>(inputs);
			// comparator + expected result
			auto comp = std::get<1>(inputs);
			auto expected = std::get<3>(inputs);
			// create dates
			oa::time::Date dt1{y1, m1, d1};
			oa::time::Date dt2{y2, m2, d2};
			// check based on expected result
			EXPECT_EQ(expected, comp(dt1, dt2)) <<
				"error: " << expected << " != (" <<
				dt1 << " " <<
				oa::testing::binary_format_traits<C>::op_string << " " <<
				dt2 << ")";
		}
	};

	/**
	 * Partial specialization for `std::tuple<int, const char*, int>`.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<J, std::tuple<int, const char*, int>> {
		/**
		 * Check that the `Date` has the right string format and day of week.
		 *
		 * This uses the `Date(int)` ctor to construct from the Julian day.
		 */
		void operator()() const
		{
			// input, string format, day of week
			auto [jdn, str, dow] = std::get<J>(date_template_test_inputs);
			// construct + check
			oa::time::Date date{jdn};
			EXPECT_EQ(str, date.ToString());
			EXPECT_EQ(dow, date.GetDOWInt());
		}
	};

	/**
	 * Partial specialization for Gregorian date and time point testing.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<
		J,
		std::pair<std::tuple<int, int, int>, std::chrono::days>
	> {
		/**
		 * Check that the `Date` corresponds to the given Unix epoch offset.
		 *
		 * @note The Unix epoch is Jan 1, 1970.
		 */
		void operator()() const
		{
			// Gregorian date triple + days since epoch
			auto [ymd, offset] = std::get<J>(date_template_test_inputs);
			auto [y, m, d] = ymd;
			// create dates
			oa::time::Date d1{y, m, d};
			// note: value-initialized time_point is the Unix epoch
			oa::time::Date d2{std::chrono::system_clock::time_point{} + offset};
			EXPECT_EQ(d1, d2);
		}
	};

	/**
	 * Traits helper to indicate supported `Date` arithmetic operators.
	 *
	 * @tparam F Binary invocable
	 */
	template <typename F>
	static constexpr bool is_date_arithmetic_op_v = (
		std::is_same_v<F, std::plus<>> ||
		std::is_same_v<F, std::minus<>>
	);

	/**
	 * Partial specialization for the old `AddTenor()` and `SubTenor()` tests.
	 *
	 * @tparam J Input index
	 * @tparam F `std::plus<>` or `std::minus<>`
	 */
	template <std::size_t J, typename F>
	requires (is_date_arithmetic_op_v<F>)
	struct dispatch_type<
		J,
		std::tuple<
			std::tuple<int, int, int>,  // input date
			F,                          // +/-
			const char*,                // tenor string
			std::equal_to<>,            // =
			const char*                 // expected date format string
		>
	> {
		/**
		 * Check that `AddTenor()` works as expected.
		 */
		void operator()() const
		{
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// construct date from input YMD + get tenor
			auto [y, m, d] = std::get<0>(inputs);
			oa::time::Date d1{y, m, d};
			oa::time::Tenor t1{std::get<2>(inputs)};
			// create new date using AddTenor() or SubTenor()
			auto d2 = [&d1, &t1]
			{
				if constexpr (std::is_same_v<F, std::plus<>>)
					return d1.AddTenor(t1);
				else
					return d1.SubTenor(t1);
			}();
			// check
			EXPECT_EQ(std::get<4>(inputs), d2.ToString());
		}
	};

	/**
	 * Traits helper for supported `Date` arithmetic operator arguments.
	 *
	 * These are not the actual arguments but types in the test input list.
	 *
	 * @tparam T `int` or `std::pair<int, oa::time::Tenors>`
	 */
	template <typename T>
	static constexpr bool is_date_arithmetic_arg_v = (
		std::is_same_v<T, int> ||
		std::is_same_v<T, std::pair<int, oa::time::Tenors>>
	);

	/**
	 * Partial specialization for adding/subtracting tenors/days from dates.
	 *
	 * @tparam J Input index
	 * @tparam F `std::plus<>` or `std::minus<>`
	 * @tparam T `std::pair<int, oa::time::Tenors` or `int`
	 */
	template <std::size_t J, typename F, typename T>
	requires (is_date_arithmetic_op_v<F> && is_date_arithmetic_arg_v<T>)
	struct dispatch_type<
		J,
		std::tuple<
			std::tuple<int, int, int>,         // input date
			F,                                 // +/-
			T,                                 // tenor/int
			std::equal_to<>,                   // =
			std::tuple<int, int, int>          // expected date
		>
	> {
		/**
		 * Check that adding a tenor to a date works as expected.
		 */
		void operator()() const
		{
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// input + output YMD
			auto [y1, m1, d1] = std::get<0>(inputs);
			auto [y2, m2, d2] = std::get<4>(inputs);
			// create dates + tenor
			oa::time::Date dt1{y1, m1, d1};
			oa::time::Date dt2{y2, m2, d2};
			// get tenor or day count
			// note: can't implicitly capture inputs (compiler deficiency)
			auto t1 = [v = std::get<2>(inputs)]
			{
				// day count
				if constexpr (std::is_same_v<T, int>)
					return v;
				// tenor inputs
				else
					return oa::time::Tenor{v.first, v.second};
			}();
			// check
			// note: could invoke F{} directly but for nicer error formatting
			// we would need to have a better way of representing the F binary
			// operation as a string in the error message
			if constexpr (std::is_same_v<F, std::plus<>>)
				EXPECT_EQ(dt2, dt1 + t1);
			else if constexpr (std::is_same_v<F, std::minus<>>)
				EXPECT_EQ(dt2, dt1 - t1);
			// guarded by requires() and uses function signature for relatively
			// quick and dirty inclusion of the F type in the message
			else
				GTEST_FAIL() << OA_PRETTY_FUNCTION_NAME <<
					": unknown binary functor provided";
		}
	};

	/**
	 * Partial specialization for adding tenors/days to dates.
	 *
	 * @tparam J Input index
	 * @tparam T `std::pair<int, oa::time::Tenors` or `int`
	 */
	template <std::size_t J, typename T>
	requires (is_date_arithmetic_arg_v<T>)
	struct dispatch_type<
		J,
		std::tuple<
			T,                                 // tenor/int
			std::plus<>,                       // +
			std::tuple<int, int, int>,         // input date
			std::equal_to<>,                   // =
			std::tuple<int, int, int>          // expected date
		>
	> {
		/**
		 * Check that adding a date to a tenor or day count works as expected.
		 */
		void operator()() const
		{
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// input + output YMD
			auto [y1, m1, d1] = std::get<2>(inputs);
			auto [y2, m2, d2] = std::get<4>(inputs);
			// create dates
			oa::time::Date dt1{y1, m1, d1};
			oa::time::Date dt2{y2, m2, d2};
			// get tenor or day count
			// note: can't implicitly capture inputs (compiler deficiency)
			auto t1 = [v = std::get<0>(inputs)]
			{
				// day count
				if constexpr (std::is_same_v<T, int>)
					return v;
				// tenor inputs
				else
					return oa::time::Tenor{v.first, v.second};
			}();
			// check
			EXPECT_EQ(dt2, t1 + dt1);
		}
	};

protected:
	/**
	 * Run the appropriate test dispatcher on the test case.
	 */
	void operator()() const
	{
		// note: use std::decay_t to remove cv-qualifiers + decay arrays
		dispatch_type<I, std::decay_t<std::tuple_element_t<I, inputs_type>>>{}();
	}
};

// instantiate with DateTest input index types
TYPED_TEST_SUITE(DateTest, oa::testing::index_types<DateTest<>::inputs_size>);

/**
 * Evaluate the `DateTest` test.
 */
TYPED_TEST(DateTest, Test)
{
	(*this)();
}

}  // namespace
