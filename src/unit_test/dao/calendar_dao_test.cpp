#include <cstdlib>
#include <string>
#include <vector>

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
			// TODO: make this a macro to allow reuse
			if (!std::getenv("OdinBaseDir"))
				OA_GTEST_SKIP() << "OdinBaseDir was not set";
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