#include "gtest/gtest.h"

#include "time/date.h"
#include "time/day_count/day_count_base.h"
#include "time/day_count/day_count_30_360_bond_basis.h"

namespace
{
	class DayCount30360Test : public ::testing::Test
	{
		public:
			oa::time::DayCount30360BondBasis day_counter_30360;
			
			
			virtual void SetUp() override
			{
				day_counter_30360 = oa::time::DayCount30360BondBasis();
			}

			virtual void TearDown() override
			{
				//this is to remove any heap allocated stuff from SetUp()
			}
	};

	TEST_F(DayCount30360Test, DayCountTest)
	{
		oa::time::Date day1, day2;

		// first test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 30);
		EXPECT_EQ(15, day_counter_30360.DayCount(day1, day2));

		//second test
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 2, 15);
		EXPECT_EQ(30, day_counter_30360.DayCount(day1, day2));

		//third test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 7, 15);
		EXPECT_EQ(180, day_counter_30360.DayCount(day1, day2));

		//fourth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_EQ(180, day_counter_30360.DayCount(day1, day2));

		//fifth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2007, 10, 31);
		EXPECT_EQ(30, day_counter_30360.DayCount(day1, day2));

		//sixth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 9, 30);
		EXPECT_EQ(360, day_counter_30360.DayCount(day1, day2));

		//seventh test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 31);
		EXPECT_EQ(16, day_counter_30360.DayCount(day1, day2));

		//eigth test 
		day1 = oa::time::Date(2007, 1, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_EQ(28, day_counter_30360.DayCount(day1, day2));

		//ninth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 31);
		EXPECT_EQ(33, day_counter_30360.DayCount(day1, day2));

		//tenth test 
		day1 = oa::time::Date(2006, 8, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_EQ(178, day_counter_30360.DayCount(day1, day2));

		//evenlth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 8, 31);
		EXPECT_EQ(183, day_counter_30360.DayCount(day1, day2));

		//twelfth test 
		day1 = oa::time::Date(2007, 2, 14);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_EQ(14, day_counter_30360.DayCount(day1, day2));

		//thirteen test 
		day1 = oa::time::Date(2007, 2, 26);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_EQ(363, day_counter_30360.DayCount(day1, day2));

		//fourteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_EQ(359, day_counter_30360.DayCount(day1, day2));

		//fifteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 30);
		EXPECT_EQ(31, day_counter_30360.DayCount(day1, day2));

		//sixteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_EQ(32, day_counter_30360.DayCount(day1, day2));

		//seventeenth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 5);
		EXPECT_EQ(7, day_counter_30360.DayCount(day1, day2));

		//eighteenth test 
		day1 = oa::time::Date(2007, 10, 31);
		day2 = oa::time::Date(2007,11, 28);
		EXPECT_EQ(28, day_counter_30360.DayCount(day1, day2));

		//nineteenth test 
		day1 = oa::time::Date(2007, 8, 31);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_EQ(179, day_counter_30360.DayCount(day1, day2));

		//twentieth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 8, 31);
		EXPECT_EQ(182, day_counter_30360.DayCount(day1, day2));

		//twenty first test 
		day1 = oa::time::Date(2008, 8, 31);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_EQ(178, day_counter_30360.DayCount(day1, day2));

		//twenty second test 
		day1 = oa::time::Date(2009, 2, 28);
		day2 = oa::time::Date(2009, 8, 31);
		EXPECT_EQ(183, day_counter_30360.DayCount(day1, day2));
	}

	TEST_F(DayCount30360Test, YearFractionTest)
	{
		oa::time::Date day1, day2;

		// first test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 30);
		EXPECT_DOUBLE_EQ(15.0/360, day_counter_30360.YearFraction(day1, day2));

		//second test
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 2, 15);
		EXPECT_DOUBLE_EQ(30.0/360, day_counter_30360.YearFraction(day1, day2));

		//third test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 7, 15);
		EXPECT_DOUBLE_EQ(.50, day_counter_30360.YearFraction(day1, day2));

		//fourth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_DOUBLE_EQ(.50, day_counter_30360.YearFraction(day1, day2));

		//fifth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2007, 10, 31);
		EXPECT_DOUBLE_EQ(30.0/360, day_counter_30360.YearFraction(day1, day2));

		//sixth test 
		day1 = oa::time::Date(2007, 9, 30);
		day2 = oa::time::Date(2008, 9, 30);
		EXPECT_DOUBLE_EQ(1.0, day_counter_30360.YearFraction(day1, day2));

		//seventh test 
		day1 = oa::time::Date(2007, 1, 15);
		day2 = oa::time::Date(2007, 1, 31);
		EXPECT_DOUBLE_EQ(16.0/360, day_counter_30360.YearFraction(day1, day2));

		//eigth test 
		day1 = oa::time::Date(2007, 1, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_DOUBLE_EQ(28.0/360, day_counter_30360.YearFraction(day1, day2));

		//ninth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 31);
		EXPECT_DOUBLE_EQ(33.0/360, day_counter_30360.YearFraction(day1, day2));

		//tenth test 
		day1 = oa::time::Date(2006, 8, 31);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_DOUBLE_EQ(178.0/360, day_counter_30360.YearFraction(day1, day2));

		//evenlth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 8, 31);
		EXPECT_DOUBLE_EQ(183.0/360, day_counter_30360.YearFraction(day1, day2));

		//twelfth test 
		day1 = oa::time::Date(2007, 2, 14);
		day2 = oa::time::Date(2007, 2, 28);
		EXPECT_DOUBLE_EQ(14.0/360, day_counter_30360.YearFraction(day1, day2));

		//thirteen test 
		day1 = oa::time::Date(2007, 2, 26);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_DOUBLE_EQ(363.0/360, day_counter_30360.YearFraction(day1, day2));

		//fourteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_DOUBLE_EQ(359.0/360, day_counter_30360.YearFraction(day1, day2));

		//fifteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 30);
		EXPECT_DOUBLE_EQ(31.0/360, day_counter_30360.YearFraction(day1, day2));

		//sixteenth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 3, 31);
		EXPECT_DOUBLE_EQ(32.0/360, day_counter_30360.YearFraction(day1, day2));

		//seventeenth test 
		day1 = oa::time::Date(2007, 2, 28);
		day2 = oa::time::Date(2007, 3, 5);
		EXPECT_DOUBLE_EQ(7.0/360, day_counter_30360.YearFraction(day1, day2));

		//eighteenth test 
		day1 = oa::time::Date(2007, 10, 31);
		day2 = oa::time::Date(2007, 11, 28);
		EXPECT_DOUBLE_EQ(28.0/360, day_counter_30360.YearFraction(day1, day2));

		//nineteenth test 
		day1 = oa::time::Date(2007, 8, 31);
		day2 = oa::time::Date(2008, 2, 29);
		EXPECT_DOUBLE_EQ(179.0/360, day_counter_30360.YearFraction(day1, day2));

		//twentieth test 
		day1 = oa::time::Date(2008, 2, 29);
		day2 = oa::time::Date(2008, 8, 31);
		EXPECT_DOUBLE_EQ(182.0/360, day_counter_30360.YearFraction(day1, day2));

		//twenty first test 
		day1 = oa::time::Date(2008, 8, 31);
		day2 = oa::time::Date(2009, 2, 28);
		EXPECT_DOUBLE_EQ(178.0/360, day_counter_30360.YearFraction(day1, day2));

		//twenty second test 
		day1 = oa::time::Date(2009, 2, 28);
		day2 = oa::time::Date(2009, 8, 31);
		EXPECT_DOUBLE_EQ(183.0/360, day_counter_30360.YearFraction(day1, day2));
	}
}