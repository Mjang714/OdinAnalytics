#include "gtest/gtest.h"

#include<vector>

#include "time/calendar_data_struct.h"
#include "time/calendar.h"
#include "time/date.h"
#include "time/time_enums.h"
#include "time/date_adjust/date_adjust_following.h"

namespace 
{
	class DateAdjustFollowingTest : public ::testing::Test
	{
		protected:
			oa::time::DateAdjustFollowing date_adjust_following_test;
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
				date_adjust_following_test = oa::time::DateAdjustFollowing(test_calendar);

			}

			virtual void TearDown() override
			{

			}
	};

	TEST_F(DateAdjustFollowingTest, NoAdjust)
	{
			oa::time::Date valid_biz_day(2022,11,10);
			oa::time::Date adjust_result = date_adjust_following_test.AdjustDate(valid_biz_day);

			EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(DateAdjustFollowingTest, AdjustHoliday)
	{
		oa::time::Date invalid_biz_day(2022, 11, 11);
		oa::time::Date valid_biz_day(2022, 11, 14);
		oa::time::Date adjust_result = date_adjust_following_test.AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(DateAdjustFollowingTest, AdjustWeekend)
	{
		oa::time::Date invalid_biz_day(2022, 10, 1);
		oa::time::Date valid_biz_day(2022, 10, 3);
		oa::time::Date adjust_result = date_adjust_following_test.AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(DateAdjustFollowingTest, AdjustMonthEnd)
	{
		oa::time::Date invalid_biz_day(2022, 7, 30);
		oa::time::Date valid_biz_day(2022, 8, 1);
		oa::time::Date adjust_result = date_adjust_following_test.AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}
}