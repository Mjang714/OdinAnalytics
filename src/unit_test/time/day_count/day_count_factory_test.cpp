#include "gtest/gtest.h"

#include "time/date.h"
#include "time/day_count/day_count_base.h"
#include "time/day_count/day_count_30_360_bond_basis.h"
#include "time/day_count/day_count_30_360e_isda.h"
#include "time/day_count/day_count_30_e_360_eurobond_basis.h"
#include "time/day_count/day_count_act_360.h"
#include "time/day_count/day_count_act_act.h  "
#include "time/day_count/day_count_act_365_fixed.h"
#include "time/day_count/day_counter_factory.h"

namespace
{
	class FactoryTestDayCount : public::testing::Test
	{
		public: 
			std::unique_ptr <oa::time::DayCounterBase> day_count_act_act;
			std::unique_ptr <oa::time::DayCounterBase> day_count_act_360;
			std::unique_ptr <oa::time::DayCounterBase> day_count_act_365f;
			std::unique_ptr <oa::time::DayCounterBase> day_count_30_360;
			std::unique_ptr <oa::time::DayCounterBase> day_count_30E_360;
			std::unique_ptr <oa::time::DayCounterBase> day_count_30E_360_isda;
		
			oa::time::Date start_date;
			oa::time::Date end_date;

			void SetUp() override
			{
				day_count_act_act = oa::time::DayCounterFactory::GenerateDayCounter(oa::time::DayCountRule::kACT_ACT);
				day_count_act_360 = oa::time::DayCounterFactory::GenerateDayCounter(oa::time::DayCountRule::kACT_360);
				day_count_act_365f = oa::time::DayCounterFactory::GenerateDayCounter(oa::time::DayCountRule::kACT_365_FIXED);
				day_count_30_360 = oa::time::DayCounterFactory::GenerateDayCounter(oa::time::DayCountRule::k30_360_BOND_BASIS);
				day_count_30E_360 = oa::time::DayCounterFactory::GenerateDayCounter(oa::time::DayCountRule::k30_360_E_EUROBOND);
				day_count_30E_360_isda = oa::time::DayCounterFactory::GenerateDayCounter(oa::time::DayCountRule::k30_E_360_ISDA);

				start_date = oa::time::Date(2007, 2, 28);
				end_date = oa::time::Date(2007, 3, 31);
			}

			void TearDown() override
			{

			}
	};

	TEST_F(FactoryTestDayCount, DayCountTest)
	{
		//check normal dates work
		EXPECT_EQ(31, day_count_act_act->DayCount(start_date, end_date));
		EXPECT_EQ(31, day_count_act_360->DayCount(start_date, end_date));
		EXPECT_EQ(31, day_count_act_365f->DayCount(start_date, end_date));
		EXPECT_EQ(33, day_count_30_360->DayCount(start_date, end_date));
		EXPECT_EQ(32, day_count_30E_360->DayCount(start_date, end_date));
		EXPECT_EQ(30, day_count_30E_360_isda->DayCount(start_date, end_date));

		//check the reverse dates
		EXPECT_EQ(-31, day_count_act_act->DayCount(end_date, start_date));
		EXPECT_EQ(-31, day_count_act_360->DayCount(end_date, start_date));
		EXPECT_EQ(-31, day_count_act_365f->DayCount(end_date, start_date));
		EXPECT_EQ(-33, day_count_30_360->DayCount(end_date, start_date));
		EXPECT_EQ(-32, day_count_30E_360->DayCount(end_date, start_date));
		EXPECT_EQ(-30, day_count_30E_360_isda->DayCount(end_date, start_date));
	}

	TEST_F(FactoryTestDayCount, YearFractionTest)
	{
		EXPECT_DOUBLE_EQ(31 / 360.0, day_count_act_360->YearFraction(start_date, end_date));
		EXPECT_DOUBLE_EQ(31 / 365.0, day_count_act_365f->YearFraction(start_date, end_date));
		EXPECT_DOUBLE_EQ(33 / 360.0, day_count_30_360->YearFraction(start_date, end_date));
		EXPECT_DOUBLE_EQ(32 / 360.0, day_count_30E_360->YearFraction(start_date, end_date));
		EXPECT_DOUBLE_EQ(30 / 360.0, day_count_30E_360_isda->YearFraction(start_date, end_date));
	}
}