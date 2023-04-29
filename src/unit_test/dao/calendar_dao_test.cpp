#include "gtest/gtest.h"

#include <string>
#include<vector>


#include "dao/calendar_dao.h"
#include "time/time_enums.h"
#include "time/calendar_data_struct.h"


namespace
{
	namespace time = oa::time;
	class CalendarDaoTest : public ::testing::Test
	{
	protected:
		std::string region_str = "NYB";
		std::vector<time::Weekdays> expected_weekend{ time::Weekdays::kSaturday, time::Weekdays::kSunday};
		oa::ds::CalendarDataStruct data_struct;
		void SetUp() override
		{
			data_struct = oa::dao::CalendarDao::GetInstance().GetCalendartData(region_str);
		}

		void TearDown() override
		{

		}
	};

	TEST_F(CalendarDaoTest, WeekendLoaded)
	{
		EXPECT_EQ(expected_weekend, data_struct.weekends);
	}

	TEST_F(CalendarDaoTest, RegionName)
	{
		EXPECT_EQ(region_str, data_struct.regions);
	}

	
}