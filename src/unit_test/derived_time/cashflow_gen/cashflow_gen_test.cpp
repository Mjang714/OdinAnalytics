#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "time/date.h"
#include "time/calendar.h"
#include "time/tenor.h"
#include "time/day_count/day_counter_factory.h"

#include "derived_time/cashflow_gen/cashflow_gen.h"
#include "derived_time/derived_time_enums.h"

namespace dt = oa::derived_time;
using ::testing::ContainerEq;

namespace
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
				  day_cnt_rule(oa::time::DayCountRule::kACT_360)
			{
			}

		protected:
			oa::time::Date start_date;
			oa::time::Date mat_date;
			oa::time::Tenor frequency;
			double notional;
			double rate;
			oa::time::DayCountRule day_cnt_rule;
			
			std::vector<oa::derived_time::CashflowStruct> fixed_cf_base{
				{"2025-1-3","2025-7-3","2025-1-3","2025-7-3","2025-1-3","2025-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2025-7-3","2026-1-3","2025-7-3","2026-1-3","2025-7-3","2026-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-1-3","2026-7-3","2026-1-3","2026-7-3","2026-1-3","2026-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-7-3","2027-1-3","2026-7-3","2027-1-3","2026-7-3","2027-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-1-3","2027-7-3","2027-1-3","2027-7-3","2027-1-3","2027-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-7-3","2028-1-3","2027-7-3","2028-1-3","2027-7-3","2028-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-1-3","2028-7-3","2028-1-3","2028-7-3","2028-1-3","2028-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-7-3","2029-1-3","2028-7-3","2029-1-3","2028-7-3","2029-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-1-3","2029-7-3","2029-1-3","2029-7-3","2029-1-3","2029-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-1-3","2029-7-3","2030-1-3","2029-7-3","2030-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-1-3","2029-7-3","2030-1-3","2029-7-3","2030-1-3", 1000000.0, 1.0, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kPrincipal }

			};
			virtual void SetUp() override
			{
				start_date = oa::time::Date(2025, 1, 3);
				mat_date = oa::time::Date(2030, 1, 3);
				frequency = oa::time::Tenor("6M");
				notional = 1000000.0;
				rate = 0.05; //5%
				day_cnt_rule = oa::time::DayCountRule::kACT_360;

				auto day_count = oa::time::DayCounterFactory::GenerateDayCounter(day_cnt_rule);
				for(auto& cf : fixed_cf_base) {
					cf.days = day_count->DayCount(cf.start_date, cf.end_date);
					cf.day_count_fraction = day_count->YearFraction(cf.start_date, cf.end_date);
					cf.cashflow_amount = cf.notional * (cf.rate * cf.day_count_fraction);
				}
				fixed_cf_base.at(10).cashflow_amount = notional;
			}
			virtual void TearDown() override
			{
			}
	};

	TEST_F(CashflowGenTest, CreateFixedCashflowsFwdTest)
	{
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::Currency::kUSD,
			oa::derived_time::DateDirection::kForward
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_base));
	}

	TEST_F(CashflowGenTest, CreateFixedCashflowsBkwdTest)
	{
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::Currency::kUSD,
			oa::derived_time::DateDirection::kBackward
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_base));
	}
}