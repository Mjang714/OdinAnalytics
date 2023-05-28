#include "gtest/gtest.h"

#include <vector>

#include "time/calendar_data_struct.h"
#include "time/calendar.h"
#include "time/date.h"
#include "time/time_enums.h"
#include "time/date_adjust/adjustment_factory.h"

namespace
{
	class AdjustmentFactoryTest : public ::testing::Test 
	{
		protected:
			std::unique_ptr<oa::time::DateAdjustBase> adj_base_mf;
			std::unique_ptr<oa::time::DateAdjustBase> adj_base_p;
			oa::time::Calendar test_calendar;

			void SetUp() override
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
				
				//create a adjustment factory and use the modified follwing test to see if it works
				adj_base_mf = oa::time::AdjustmentFactory::CreateDateAdjust(oa::time::AdjRule::kModifiedFollowing, test_calendar);
				
				//use the Adjustment factory to create a previous one and test that works against the previous case 
				adj_base_p = oa::time::AdjustmentFactory::CreateDateAdjust(oa::time::AdjRule::kPreceding, test_calendar);
			}

			void TearDown() override
			{

			}
	};

	TEST_F(AdjustmentFactoryTest, NoAdjustMF)
	{
		oa::time::Date valid_biz_day(2022, 11, 10);

		oa::time::Date adjust_result = adj_base_mf->AdjustDate(valid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(AdjustmentFactoryTest, AdjustHolidayMF)
	{
		oa::time::Date invalid_biz_day(2022, 11, 11);
		oa::time::Date valid_biz_day(2022, 11, 14);

		oa::time::Date adjust_result = adj_base_mf->AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(AdjustmentFactoryTest, AdjustWeekendMF)
	{
		oa::time::Date invalid_biz_day(2022, 11, 19);
		oa::time::Date valid_biz_day(2022, 11, 21);

		oa::time::Date adjust_result = adj_base_mf->AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(AdjustmentFactoryTest, AdjustMonthEndMF)
	{
		oa::time::Date invalid_biz_day(2022, 7, 30);
		oa::time::Date valid_biz_day(2022, 7, 29);

		oa::time::Date adjust_result = adj_base_mf->AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}
	
	TEST_F(AdjustmentFactoryTest, NoAdjustP)
	{
		oa::time::Date valid_biz_day(2022, 11, 10);

		oa::time::Date adjust_result = adj_base_p->AdjustDate(valid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(AdjustmentFactoryTest, AdjustHolidayP)
	{
		oa::time::Date invalid_biz_day(2022, 11, 12);
		oa::time::Date valid_biz_day(2022, 11, 10);

		oa::time::Date adjust_result = adj_base_p->AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(AdjustmentFactoryTest, AdjustWeekendP)
	{
		oa::time::Date invalid_biz_day(2022, 11, 20);
		oa::time::Date valid_biz_day(2022, 11, 18);

		oa::time::Date adjust_result = adj_base_p->AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}

	TEST_F(AdjustmentFactoryTest, AdjustMonthEndP)
	{
		oa::time::Date invalid_biz_day(2022, 7, 31);
		oa::time::Date valid_biz_day(2022, 7, 29);

		oa::time::Date adjust_result = adj_base_p->AdjustDate(invalid_biz_day);

		EXPECT_EQ(valid_biz_day, adjust_result);
	}
}