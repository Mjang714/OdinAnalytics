
#include "gtest/gtest.h"

#include<vector>

#include "time/calendar_data_struct.h"
#include "time/calendar.h"
#include "time/date.h"
#include "time/time_enums.h"

namespace
{
	class CalendarUnitTest : public ::testing::Test
	{
		protected:

			oa::time::Calendar test_calendar;
			virtual void SetUp() override
			{
				oa::ds::CalendarDataStruct raw_calendar_data{};
				//populate some New York Banking holidays 
				raw_calendar_data.list_of_holidays.push_back(oa::time::Date("2022-5-30"));
				raw_calendar_data.list_of_holidays.push_back(oa::time::Date("2022/7/4"));
				raw_calendar_data.list_of_holidays.push_back(oa::time::Date(2022, 6, 20));
				raw_calendar_data.list_of_holidays.push_back(oa::time::Date(2022, 9, 5));
				raw_calendar_data.list_of_holidays.push_back(oa::time::Date(2022, 10, 10));
				raw_calendar_data.list_of_holidays.push_back(oa::time::Date(2022, 11, 11));

				//populate the region that the calendar represents 
				raw_calendar_data.regions = "NYC";

				//popualte the weekends
				raw_calendar_data.weekends.push_back(oa::time::Weekdays::kSaturday);
				raw_calendar_data.weekends.push_back(oa::time::Weekdays::kSunday);

				std::vector<oa::ds::CalendarDataStruct> calendar_data_collection{ raw_calendar_data };

				test_calendar = oa::time::Calendar(calendar_data_collection);

			}

			virtual void TearDown() override
			{

			}


	};

	TEST_F(CalendarUnitTest, IsHoliday)
	{
		//test a random Weekend and Weekday 
		oa::time::Date WeekendTest(2022, 8, 13);
		oa::time::Date WeekdayTest(2022, 1, 31);

		EXPECT_TRUE(test_calendar.IsHoliday(WeekendTest));
		EXPECT_FALSE(test_calendar.IsHoliday(WeekdayTest));

		//test a really long weekend in the past and future 
		oa::time::Date old_sunday(1980, 7, 6);
		oa::time::Date far_sunday(2150, 7, 11);

		EXPECT_TRUE(test_calendar.IsHoliday(old_sunday));
		EXPECT_TRUE(test_calendar.IsHoliday(far_sunday));

		//Test special given holidays 
		oa::time::Date memorial_day_22("2022-5-30");
		oa::time::Date july_fourth_22("2022/7/4");
		oa::time::Date junetenth_22(2022, 6, 20);

		EXPECT_TRUE(test_calendar.IsHoliday(memorial_day_22));
		EXPECT_TRUE(test_calendar.IsHoliday(july_fourth_22));
		EXPECT_TRUE(test_calendar.IsHoliday(junetenth_22));

	}

	TEST_F(CalendarUnitTest, IsBusinessDay)
	{
		//test a random Weekend and Weekday 
		oa::time::Date WeekendTest(2022, 8, 13);
		oa::time::Date WeekdayTest(2022, 1, 31);

		EXPECT_FALSE(test_calendar.IsBusinessDay(WeekendTest));
		EXPECT_TRUE(test_calendar.IsBusinessDay(WeekdayTest));
	}

	TEST_F(CalendarUnitTest, GetNextBusinessDay)
	{
		//test juneteenth 
		oa::time::Date day_before_juneteenth(2022, 6, 17);
		oa::time::Date juneteenth_test_result = test_calendar.GetNextBusinessDay(day_before_juneteenth);
		EXPECT_EQ("2022-6-21 : Julian Integer = 2459752" , juneteenth_test_result.ToString());

		//Test Veterans day 
		oa::time::Date day_before_veterans(2022, 11, 10);
		oa::time::Date veterans_test_result = test_calendar.GetNextBusinessDay(day_before_veterans);
		EXPECT_EQ("2022-11-14 : Julian Integer = 2459898", veterans_test_result.ToString());

		//Test a weekend give the the next businessday
		oa::time::Date weekend_date(2022, 8, 28);
		oa::time::Date weekend_date_test = test_calendar.GetNextBusinessDay(weekend_date);
		EXPECT_EQ("2022-8-29 : Julian Integer = 2459821", weekend_date_test.ToString());

	}

	TEST_F(CalendarUnitTest, GetPrevBusinessDay)
	{
		//test juneteenth 
		oa::time::Date day_after_juneteenth(2022, 6, 21);
		oa::time::Date juneteenth_test_result = test_calendar.GetPrevBusinessDay(day_after_juneteenth);
		EXPECT_EQ("2022-6-17 : Julian Integer = 2459748", juneteenth_test_result.ToString());

		//Test a weekend give the the next businessday
		oa::time::Date weekend_date(2022, 8, 28);
		oa::time::Date weekend_date_test = test_calendar.GetPrevBusinessDay(weekend_date);
		EXPECT_EQ("2022-8-26 : Julian Integer = 2459818", weekend_date_test.ToString());
	}
}