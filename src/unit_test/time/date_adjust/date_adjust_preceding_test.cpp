#include "gtest/gtest.h"

#include<vector>

#include "time/calendar_data_struct.h"
#include "time/calendar.h"
#include "time/date.h"
#include "time/time_enums.h"
#include "time/date_adjust/date_adjust_preceding.h"

namespace
{
	class DateAdjustPrecedingTest : public ::testing::Test
	{
		protected:
			oa::time::Calendar test_calendar;
			oa::time::DateAdjustPreceding date_adjust_preceding_test;

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
				date_adjust_preceding_test = oa::time::DateAdjustPreceding(test_calendar);
			}

			virtual void TearDown() override
			{

			}
	};

	TEST_F(DateAdjustPrecedingTest, NoAdjust)
	{
		oa::time::Date valid_biz_day(2022, 11, 10);

		oa::time::Date adjust_result = date_adjust_preceding_test.AdjustDate(valid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(DateAdjustPrecedingTest, AdjustHoliday)
	{
		oa::time::Date invalid_biz_day(2022, 11, 12);
		oa::time::Date valid_biz_day(2022, 11, 10);

		oa::time::Date adjust_result = date_adjust_preceding_test.AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(DateAdjustPrecedingTest, AdjustWeekend)
	{
		oa::time::Date invalid_biz_day(2022, 11, 20);
		oa::time::Date valid_biz_day(2022, 11, 18);

		oa::time::Date adjust_result = date_adjust_preceding_test.AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(DateAdjustPrecedingTest, AdjustMonthEnd)
	{
		oa::time::Date invalid_biz_day(2022, 7, 31);
		oa::time::Date valid_biz_day(2022, 7, 29);

		oa::time::Date adjust_result = date_adjust_preceding_test.AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}
}