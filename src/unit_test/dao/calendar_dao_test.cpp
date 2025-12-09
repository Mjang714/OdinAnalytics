#include <cstdlib>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "dao/calendar_dao.h"
#include "oa/testing/gtest.h"
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
			// skip test if OdinBaseDir was not set correctly
			OA_GTEST_ENSURE_BASE_DIR();
			data_struct = oa::dao::CalendarDao::GetInstance().GetCalendartData(region_str);
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