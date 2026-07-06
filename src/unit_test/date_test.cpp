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
#include <tuple>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "oa/testing/gtest.h"  // for oa::testing::index_types
#include "oa/time/tenor.h"     // for oa::time::Tenor

namespace {

/**
 * `Date` class test fixture.
 */
class DateTest : public ::testing::Test {};

// TODO: revisit testing infra to enable more thorough testing

// DateTemplateTest inputs for constructor testing
constexpr auto date_template_test_inputs = std::make_tuple(
	// original bad inputs
	std::pair{"190O-12-04", false},
	std::pair{"19-10-2004", false},
	std::pair{"12-10-2007", false},
	// test delimiter consistency
	std::pair{"2020/09-24", false},
	std::pair{"2025:1-23", false},
	// original valid inputs
	std::pair{"1900-12-04", true},
	std::pair{"1900:12:04", true},
	std::pair{"1900/12/04", true},
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
	// original AddTenor() tests
	std::tuple{
		std::tuple{2022, 7, 29},
		"2D",
		"2022-7-31 : Julian Integer = 2459792"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"580d",
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"0d",
		"2022-7-29 : Julian Integer = 2459790"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		"2w",
		"2022-8-11 : Julian Integer = 2459803"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		"83w",
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		"0w",
		"2022-7-28 : Julian Integer = 2459789"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"2M",
		"2022-9-29 : Julian Integer = 2459852"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"19M",
		"2024-2-29 : Julian Integer = 2460370"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"7M",
		"2023-2-28 : Julian Integer = 2460004"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"67M",
		"2028-2-29 : Julian Integer = 2461831"
	},
	// note: tests adding month to end of a month with 31st as the last day
	std::tuple{
		std::tuple{2022, 8, 31},
		"1M",
		"2022-9-30 : Julian Integer = 2459853"
	},
	std::tuple{
		std::tuple{2024, 2, 29},
		"1Y",
		"2025-2-28 : Julian Integer = 2460735"
	},
	std::tuple{
		std::tuple{2021, 9, 15},
		"60Y",
		"2081-9-15 : Julian Integer = 2481388"
	},
	std::tuple{
		std::tuple{2015, 8, 31},
		"100Y",
		"2115-8-31 : Julian Integer = 2493790"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"-2D",
		"2022-7-27 : Julian Integer = 2459788"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"-580d",
		"2020-12-26 : Julian Integer = 2459210"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"-0d",
		"2022-7-29 : Julian Integer = 2459790"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		"-2w",
		"2022-7-14 : Julian Integer = 2459775"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		"-83w",
		"2020-12-24 : Julian Integer = 2459208"
	},
	std::tuple{
		std::tuple{2022, 7, 28},
		"-0w",
		"2022-7-28 : Julian Integer = 2459789"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"-2M",
		"2022-5-29 : Julian Integer = 2459729"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"-29M",
		"2020-2-29 : Julian Integer = 2458909"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"-7M",
		"2021-12-29 : Julian Integer = 2459578"
	},
	std::tuple{
		std::tuple{2022, 7, 29},
		"-67M",
		"2016-12-29 : Julian Integer = 2457752"
	},
	// note: tests subtracting month to end of month with 31st as last day
	std::tuple{
		std::tuple{2022, 8, 31},
		"-2M",
		"2022-6-30 : Julian Integer = 2459761"
	},
	// note: tests subtracting month to end of month with 31st as last day
	std::tuple{
		std::tuple{2022, 8, 31},
		"-36M",
		"2019-8-31 : Julian Integer = 2458727"
	},
	std::tuple{
		std::tuple{2024, 2, 29},
		"-1Y",
		"2023-2-28 : Julian Integer = 2460004"
	},
	std::tuple{
		std::tuple{2081, 9, 15},
		"-60Y",
		"2021-9-15 : Julian Integer = 2459473"
	},
	std::tuple{
		std::tuple{2015, 8, 31},
		"-100Y",
		"1915-8-31 : Julian Integer = 2420741"
	},
	// testing adding tenors to dates or dates to tenors
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
	}
);

/**
 * `Date` class generic template tests.
 *
 * @tparam T `oa::testing::index<I>` where `I` indexes into the inputs array
 */
template <typename T = void>
class DateTemplateTest : public DateTest {
public:
	// size of the test input
	static constexpr auto inputs_size =
		std::tuple_size_v<decltype(date_template_test_inputs)>;
};

/**
 * Partial specialization for the actual `DateTemplateTest` instantiation.
 *
 * @tparam I Input index
 */
template <std::size_t I>
class DateTemplateTest<oa::testing::index<I>> : public DateTest {
private:
	// type of the input tuple + safety check
	using inputs_type = decltype(date_template_test_inputs);
	static_assert(I < std::tuple_size_v<inputs_type>, "I out of bounds");

	/**
	 * Test dispatcher type.
	 *
	 * This allows us to segregate logic based on the test case input type
	 * while also ensuring we can obtain the input as a constant expression.
	 *
	 * @note The extra index argument enables us to create partial
	 *  specializations by changing `T` as we cannot have explicit template
	 *  specializations in a non-namespace space in C++.
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
				EXPECT_ANY_THROW(oa::time::Date{inputs.first});
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
	 * Partial specialization for the old `AddTenor()` tests.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<
		J,
		std::tuple<
			std::tuple<int, int, int>,  // input date
			const char*,                // tenor string
			const char*                 // expected date format string
		>
	> {
		/**
		 * Check that `AddTenor()` works as expected.
		 */
		void operator()() const
		{
			constexpr auto inputs = std::get<J>(date_template_test_inputs);
			// construct date from input YMD
			auto [y, m, d] = std::get<0>(inputs);
			oa::time::Date d1{y, m, d};
			// tenor
			oa::time::Tenor t1{std::get<1>(inputs)};
			// create new date + check
			auto d2 = d1.AddTenor(t1);
			EXPECT_EQ(std::get<2>(inputs), d2.ToString());
		}
	};

	/**
	 * Partial specialization for adding dates and tenors together.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<
		J,
		std::tuple<
			std::tuple<int, int, int>,         // input date
			std::plus<>,                       // +
			std::pair<int, oa::time::Tenors>,  // tenor
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
			// input YMD, tenor inputs, output YMD
			auto [y1, m1, d1] = std::get<0>(inputs);
			auto [tc, tu] = std::get<2>(inputs);
			auto [y2, m2, d2] = std::get<4>(inputs);
			// create dates + tenor
			oa::time::Date dt1{y1, m1, d1};
			oa::time::Tenor t1{tc, tu};
			oa::time::Date dt2{y2, m2, d2};
			// check
			EXPECT_EQ(dt2, dt1 + t1);
		}
	};

	/**
	 * Partial specialization for adding tenors and dates together.
	 *
	 * @tparam J Input index
	 */
	template <std::size_t J>
	struct dispatch_type<
		J,
		std::tuple<
			std::pair<int, oa::time::Tenors>,  // tenor
			std::plus<>,                       // +
			std::tuple<int, int, int>,         // input date
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
			// tenor inputs, input YMD, output YMD
			auto [tc, tu] = std::get<0>(inputs);
			auto [y1, m1, d1] = std::get<2>(inputs);
			auto [y2, m2, d2] = std::get<4>(inputs);
			// create dates + tenor
			oa::time::Date dt1{y1, m1, d1};
			oa::time::Tenor t1{tc, tu};
			oa::time::Date dt2{y2, m2, d2};
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

/**
 * Traits type providing the `DateTemplateTest` input types.
 *
 * @tparam N Number of test cases
 */
TYPED_TEST_SUITE(
	DateTemplateTest,
	oa::testing::index_types<DateTemplateTest<>::inputs_size>
);

/**
 * Evaluate the `DateTemplateTest` test.
 */
TYPED_TEST(DateTemplateTest, Test)
{
	constexpr std::chrono::system_clock::time_point a{};
	(*this)();
}

TEST_F(DateTest, IsLeap)
{
	// test to see if the year 2000 is a leap year
	oa::time::Date test_date_1("2000-7-31");
	EXPECT_TRUE(test_date_1.IsLeap());

	// test to see if the year 2024 is a leap year
	oa::time::Date test_date_2("2024-7-31");
	EXPECT_TRUE(test_date_2.IsLeap());

	// test if the 2100 is not a leap year
	oa::time::Date test_date_3("2100-1-1");
	EXPECT_FALSE(test_date_3.IsLeap());
}

TEST_F(DateTest, DaysInMonth)
{
	using oa::time::Date;
	//Test to see if we get leap year correct days
	EXPECT_EQ(29, Date::DaysInMonth(2000, 2));
	EXPECT_EQ(28, Date::DaysInMonth(2100, 2));
	EXPECT_EQ(28, Date::DaysInMonth(2030, 2));
	EXPECT_EQ(30, Date::DaysInMonth(2010, 11));
	EXPECT_EQ(31, Date::DaysInMonth(2020, 12));
}

TEST_F(DateTest, DateOpOverloadingRelational)
{
	// test to see if the year 2000 is a leap year
	oa::time::Date test_date_1("2000-7-31");
	oa::time::Date test_date_2("2010-10-25");
	oa::time::Date test_date_3("2000-7-31");

	//test false date logic
	EXPECT_FALSE(test_date_1 == test_date_2);
	EXPECT_FALSE(test_date_1 >= test_date_2);
	EXPECT_FALSE(test_date_1 > test_date_2);
	EXPECT_FALSE(test_date_1 > test_date_3);
	EXPECT_FALSE(test_date_1 < test_date_3);

	//test true logic
	EXPECT_TRUE(test_date_1 == test_date_3);
	EXPECT_TRUE(test_date_1 <= test_date_2);
	EXPECT_TRUE(test_date_1 < test_date_2);
}

/**
 * Test adding days using `operator+`.
 */
TEST_F(DateTest, OpAddDays)
{
	using oa::time::Date;
	// base date
	Date d0{2022, 7, 29};
	// expected dates
	Date d1{2022, 7, 31};
	Date d2{2024, 2, 29};
	Date d3{2022, 7, 29};
	// check when modified by increments
	EXPECT_EQ(d1, d0 + 2);
	EXPECT_EQ(d2, 580 + d0);
	EXPECT_EQ(d3, d0 + 0);
}

TEST_F(DateTest, SubDays)
{
	oa::time::Date base_date(2022, 7, 29);
	oa::time::Tenor test_tenor_1("2D");
	oa::time::Tenor test_tenor_2("580d");
	oa::time::Tenor test_tenor_3("0d");

	oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
	EXPECT_EQ("2022-7-27 : Julian Integer = 2459788", new_date_1.ToString());

	oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
	EXPECT_EQ("2020-12-26 : Julian Integer = 2459210", new_date_2.ToString());

	oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
	EXPECT_EQ("2022-7-29 : Julian Integer = 2459790", new_date_3.ToString());
}

TEST_F(DateTest, SubWeeks)
{
	oa::time::Date base_date(2022, 7, 28);
	oa::time::Tenor test_tenor_1("2w");
	oa::time::Tenor test_tenor_2("83w");
	oa::time::Tenor test_tenor_3("0w");

	oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
	EXPECT_EQ("2022-7-14 : Julian Integer = 2459775", new_date_1.ToString());

	oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
	EXPECT_EQ("2020-12-24 : Julian Integer = 2459208", new_date_2.ToString());

	oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
	EXPECT_EQ("2022-7-28 : Julian Integer = 2459789", new_date_3.ToString());
}

TEST_F(DateTest, SubMonths)
{
	oa::time::Date base_date_1(2022, 7, 29);
	oa::time::Tenor test_tenor_1("2M");
	oa::time::Tenor test_tenor_2("29M");
	oa::time::Tenor test_tenor_3("7M");
	oa::time::Tenor test_tenor_4("67M");

	oa::time::Date new_date_1 = base_date_1.SubTenor(test_tenor_1);
	EXPECT_EQ("2022-5-29 : Julian Integer = 2459729", new_date_1.ToString());

	oa::time::Date new_date_2 = base_date_1.SubTenor(test_tenor_2);
	EXPECT_EQ("2020-2-29 : Julian Integer = 2458909", new_date_2.ToString());

	oa::time::Date new_date_3 = base_date_1.SubTenor(test_tenor_3);
	EXPECT_EQ("2021-12-29 : Julian Integer = 2459578", new_date_3.ToString());

	oa::time::Date new_date_4 = base_date_1.SubTenor(test_tenor_4);
	EXPECT_EQ("2016-12-29 : Julian Integer = 2457752", new_date_4.ToString());

	////Test adding month to end of a month wher the last day 31st
	oa::time::Date base_date_2(2022, 8, 31);
	oa::time::Tenor test_tenor_5("2M");

	oa::time::Date new_date_5 = base_date_2.SubTenor(test_tenor_5);
	EXPECT_EQ("2022-6-30 : Julian Integer = 2459761", new_date_5.ToString());

	// subtract out years
	oa::time::Tenor test_tenor_6("36M");

	oa::time::Date new_date_6 = base_date_2.SubTenor(test_tenor_6);
	EXPECT_EQ("2019-8-31 : Julian Integer = 2458727", new_date_6.ToString());
}

TEST_F(DateTest, SubYears)
{
	oa::time::Date base_date_1(2024, 2, 29);
	oa::time::Date base_date_2(2081, 9, 15);
	oa::time::Date base_date_3(2015, 8, 31);

	oa::time::Tenor test_tenor_1("1Y");
	oa::time::Tenor test_tenor_2("60Y");
	oa::time::Tenor test_tenor_3("100Y");

	EXPECT_EQ("2023-2-28 : Julian Integer = 2460004", base_date_1.SubTenor(test_tenor_1).ToString());
	EXPECT_EQ("2021-9-15 : Julian Integer = 2459473", base_date_2.SubTenor(test_tenor_2).ToString());
	EXPECT_EQ("1915-8-31 : Julian Integer = 2420741", base_date_3.SubTenor(test_tenor_3).ToString());
}

TEST_F(DateTest, SubNegDays)
{
	oa::time::Date base_date(2022, 7, 29);
	oa::time::Tenor test_tenor_1("-2D");
	oa::time::Tenor test_tenor_2("-580d");
	oa::time::Tenor test_tenor_3("-0d");

	oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
	EXPECT_EQ("2022-7-31 : Julian Integer = 2459792", new_date_1.ToString());

	oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
	EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

	oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
	EXPECT_EQ("2022-7-29 : Julian Integer = 2459790", new_date_3.ToString());
}

TEST_F(DateTest, SubNegWeeks)
{
	oa::time::Date base_date(2022, 7, 28);
	oa::time::Tenor test_tenor_1("-2w");
	oa::time::Tenor test_tenor_2("-83w");
	oa::time::Tenor test_tenor_3("-0w");

	oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
	EXPECT_EQ("2022-8-11 : Julian Integer = 2459803", new_date_1.ToString());

	oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
	EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

	oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
	EXPECT_EQ("2022-7-28 : Julian Integer = 2459789", new_date_3.ToString());
}

TEST_F(DateTest, SubNegMonths)
{
	oa::time::Date base_date_1(2022, 7, 29);
	oa::time::Tenor test_tenor_1("-2M");
	oa::time::Tenor test_tenor_2("-19M");
	oa::time::Tenor test_tenor_3("-7M");
	oa::time::Tenor test_tenor_4("-67M");

	oa::time::Date new_date_1 = base_date_1.SubTenor(test_tenor_1);
	EXPECT_EQ("2022-9-29 : Julian Integer = 2459852", new_date_1.ToString());

	oa::time::Date new_date_2 = base_date_1.SubTenor(test_tenor_2);
	EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

	oa::time::Date new_date_3 = base_date_1.SubTenor(test_tenor_3);
	EXPECT_EQ("2023-2-28 : Julian Integer = 2460004", new_date_3.ToString());

	oa::time::Date new_date_4 = base_date_1.SubTenor(test_tenor_4);
	EXPECT_EQ("2028-2-29 : Julian Integer = 2461831", new_date_4.ToString());

	//Test adding month to end of a month wher the last day 31st
	oa::time::Date base_date_2(2022, 8, 31);
	oa::time::Tenor test_tenor_5("-1M");

	oa::time::Date new_date_5 = base_date_2.SubTenor(test_tenor_5);
	EXPECT_EQ("2022-9-30 : Julian Integer = 2459853", new_date_5.ToString());

}

TEST_F(DateTest, SubNegYears)
{
	oa::time::Date base_date_1(2024, 2, 29);
	oa::time::Date base_date_2(2021, 9, 15);
	oa::time::Date base_date_3(2015, 8, 31);

	oa::time::Tenor test_tenor_1("-1Y");
	oa::time::Tenor test_tenor_2("-60Y");
	oa::time::Tenor test_tenor_3("-100Y");

	EXPECT_EQ("2025-2-28 : Julian Integer = 2460735", base_date_1.SubTenor(test_tenor_1).ToString());
	EXPECT_EQ("2081-9-15 : Julian Integer = 2481388", base_date_2.SubTenor(test_tenor_2).ToString());
	EXPECT_EQ("2115-8-31 : Julian Integer = 2493790", base_date_3.SubTenor(test_tenor_3).ToString());
}

}  // namespace
