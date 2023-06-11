#include "gtest/gtest.h"

#include "time/date.h"
#include "time/day_count/day_count_act_365_fixed.h"


namespace
{
	class DayCountAct365FixedTest : public::testing::Test
	{
		public:
			oa::time::DayCountAct365Fixed day_counter_act_365_fixed;

			void SetUp() override
			{
				day_counter_act_365_fixed = oa::time::DayCountAct365Fixed();
			}

			void TearDown() override
			{
				//remove any heap allocated objtects if there were any
			}
	};

	TEST_F(DayCountAct365FixedTest, DayCountTest)
	{
		oa::time::Date day1, day2;

		// first test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 30);
		EXPECT_EQ(15, day_counter_act_365_fixed.DayCount(day1, day2));

		//second test
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 2, 15);
		EXPECT_EQ(31, day_counter_act_365_fixed.DayCount(day1, day2));

		//third test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 7, 15);
		EXPECT_EQ(181, day_counter_act_365_fixed.DayCount(day1, day2));

		//fourth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_EQ(183, day_counter_act_365_fixed.DayCount(day1, day2));

		//fifth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2007, 10, 31);
		EXPECT_EQ(31, day_counter_act_365_fixed.DayCount(day1, day2));

		//sixth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 9, 30);
		EXPECT_EQ(366, day_counter_act_365_fixed.DayCount(day1, day2));

		//seventh test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 31);
		EXPECT_EQ(16, day_counter_act_365_fixed.DayCount(day1, day2));

		//eigth test 
		day1 = oa::time::Date(2007, 1, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_EQ(28, day_counter_act_365_fixed.DayCount(day1, day2));

		//ninth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 31);
		EXPECT_EQ(31, day_counter_act_365_fixed.DayCount(day1, day2));

		//tenth test 
		day1 = oa::time::Date(2006, 8, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_EQ(181, day_counter_act_365_fixed.DayCount(day1, day2));

		//evenlth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 8, 31);
		EXPECT_EQ(184, day_counter_act_365_fixed.DayCount(day1, day2));

		//twelfth test 
		day1 = oa::time::Date(2007, 2, 14);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_EQ(14, day_counter_act_365_fixed.DayCount(day1, day2));

		//thirteen test 
		day1 = oa::time::Date(2007, 2, 26);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_EQ(368, day_counter_act_365_fixed.DayCount(day1, day2));

		//fourteenth test (check this)
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_EQ(365, day_counter_act_365_fixed.DayCount(day1, day2));

		//fifteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 30);
		EXPECT_EQ(30, day_counter_act_365_fixed.DayCount(day1, day2));

		//sixteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_EQ(31, day_counter_act_365_fixed.DayCount(day1, day2));

		//seventeenth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 5);
		EXPECT_EQ(5, day_counter_act_365_fixed.DayCount(day1, day2));

		//eighteenth test 
		day1 = oa::time::Date(2007, 10, 31);
		day2 = oa::time::Date(2007, 11, 28);
		EXPECT_EQ(28, day_counter_act_365_fixed.DayCount(day1, day2));

		//nineteenth test 
		day1 = oa::time::Date(2007, 8, 31);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_EQ(182, day_counter_act_365_fixed.DayCount(day1, day2));

		//twentieth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 8, 31);
		EXPECT_EQ(184, day_counter_act_365_fixed.DayCount(day1, day2));

		//twenty first test (check this) 
		day1 = oa::time::Date(2008, 8, 31);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_EQ(181, day_counter_act_365_fixed.DayCount(day1, day2));

		//twenty second test 
		day1 = oa::time::Date(2009, 2, 28);
		day2 = oa::time::Date(2009, 8, 31);
		EXPECT_EQ(184, day_counter_act_365_fixed.DayCount(day1, day2));
	}

	TEST_F(DayCountAct365FixedTest, YearFractionTest)
	{
		oa::time::Date day1, day2;

		// first test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 30);
		EXPECT_DOUBLE_EQ(15/365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//second test
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 2, 15);
		EXPECT_DOUBLE_EQ(31 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//third test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 7, 15);
		EXPECT_DOUBLE_EQ(181 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//fourth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_DOUBLE_EQ(183 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//fifth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2007, 10, 31);
		EXPECT_DOUBLE_EQ(31 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//sixth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 9, 30);
		EXPECT_DOUBLE_EQ(366 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//seventh test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 31);
		EXPECT_DOUBLE_EQ(16 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//eigth test 
		day1 = oa::time::Date(2007, 1, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_DOUBLE_EQ(28 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//ninth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 31);
		EXPECT_DOUBLE_EQ(31 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//tenth test 
		day1 = oa::time::Date(2006, 8, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_DOUBLE_EQ(181 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//evenlth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 8, 31);
		EXPECT_DOUBLE_EQ(184 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//twelfth test 
		day1 = oa::time::Date(2007, 2, 14);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_DOUBLE_EQ(14 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//thirteen test 
		day1 = oa::time::Date(2007, 2, 26);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_DOUBLE_EQ(368 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//fourteenth test (check this)
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_DOUBLE_EQ(365 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//fifteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 30);
		EXPECT_DOUBLE_EQ(30 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//sixteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_DOUBLE_EQ(31 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//seventeenth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 5);
		EXPECT_DOUBLE_EQ(5 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//eighteenth test 
		day1 = oa::time::Date(2007, 10, 31);
		day2 = oa::time::Date(2007, 11, 28);
		EXPECT_DOUBLE_EQ(28 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//nineteenth test 
		day1 = oa::time::Date(2007, 8, 31);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_DOUBLE_EQ(182 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//twentieth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 8, 31);
		EXPECT_DOUBLE_EQ(184 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//twenty first test (check this) 
		day1 = oa::time::Date(2008, 8, 31);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_DOUBLE_EQ(181 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));

		//twenty second test 
		day1 = oa::time::Date(2009, 2, 28);
		day2 = oa::time::Date(2009, 8, 31);
		EXPECT_DOUBLE_EQ(184 / 365.0, day_counter_act_365_fixed.YearFraction(day1, day2));
	}
}