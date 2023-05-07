#include "gtest/gtest.h"
#include <string>

#include "static_data_cache/calendar_cache.h"



namespace
{
	class CalendarCacheTest : public ::testing::Test 
	{
		protected:
			std::string ny_calendar_str= "NYB";
			std::string lon_calendar_str = "LNB";
			virtual void SetUp() override
			{

			}

			virtual void TearDown() override
			{

			}
	};

	TEST_F(CalendarCacheTest, LoadingHolidayTestFirst)
	{
		oa::time::Date thanks_giving_test(2024,11,28);
		auto new_york_cal = oa::static_cache::CalendarCache::RetrieveCache().GetCalendar(ny_calendar_str);

		EXPECT_TRUE(new_york_cal->IsHoliday(thanks_giving_test));
	}

	TEST_F(CalendarCacheTest, LoadingHolidayTestSecond)
	{
		oa::time::Date boxing_day_test(2024, 12, 26);
		auto london_cal = oa::static_cache::CalendarCache::RetrieveCache().GetCalendar(lon_calendar_str);

		EXPECT_TRUE(london_cal->IsHoliday(boxing_day_test));
	}
}