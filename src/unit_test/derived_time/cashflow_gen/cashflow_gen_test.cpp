#include "gtest/gtest.h"

#include "time/date.h"
#include "time/calendar.h"
#include "time/tenor.h"
#include "time/day_count/day_counter_factory.h"

#include "derived_time/cashflow_gen/cashflow_gen.h"
#include "derived_time/derived_time_enums.h"

namespace oa::unit_test::derived_time
{
	class CashflowGenTest : public ::testing::Test
	{
		public:
			CashflowGenTest()
				: start_date(),
				  mat_date(),
				  frequency("6M"),
				  notional(0.0),
				  rate(0.0),
				  day_cnt_rule(oa::time::DayCountRule::kACT_ACT)
			{
			}

		protected:
			oa::time::Date start_date;
			oa::time::Date mat_date;
			oa::time::Tenor frequency;
			double notional;
			double rate;
			oa::time::DayCountRule day_cnt_rule;
			virtual void SetUp() override
			{
				start_date = oa::time::Date(2025, 1, 3);
				mat_date = oa::time::Date(2030, 1, 3);
				frequency = oa::time::Tenor("6M");
				notional = 1000000.0;
				rate = 0.05; //5%
				day_cnt_rule = oa::time::DayCountRule::kACT_ACT;
			}
			virtual void TearDown() override
			{
			}
	};

	TEST_F(CashflowGenTest, CreateFixedCashflowsFwdTest)
	{
		auto cashflows = oa::derived_time::CashflowGen::CreateCashflows(
			start_date,
			mat_date,
			frequency,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::DateDirection::kForward
		);
		EXPECT_EQ(10, cashflows.size());
	}

	TEST_F(CashflowGenTest, CreateFixedCashflowsBkwdTest)
	{
		auto cashflows = oa::derived_time::CashflowGen::CreateCashflows(
			start_date,
			mat_date,
			frequency,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::DateDirection::kBackward
		);
		EXPECT_EQ(10, cashflows.size());
	}
}