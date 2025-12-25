#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "oa/testing/gtest.h"
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
	class CashflowGenBaseTest : public ::testing::Test
	{
		public:
			CashflowGenBaseTest()
				: start_date(),
				  mat_date(),
				  frequency("6M"),
				  notional(0.0),
				  rate(0.0),
				  day_cnt_rule(oa::time::DayCountRule::kACT_360),
				  pay_calendar("NYB"),
				  fix_calendar("NYB"),
				  tenor_pay("2d"),
				  tenor_fix("-2d"),
				  adjustment_rule_pay(oa::time::AdjRule::kModifiedFollowing),
				  adjustment_rule_fix(oa::time::AdjRule::kPreceding)

			{
			}

		protected:
			oa::time::Date start_date;
			oa::time::Date mat_date;
			oa::time::Tenor frequency;
			double notional;
			double rate;
			oa::time::DayCountRule day_cnt_rule;
			oa::derived_time::DateFormula date_formula_fix, date_formula_pay;
			std::string pay_calendar;
			std::string fix_calendar;
			std::string tenor_pay;
			std::string tenor_fix;
			oa::time::AdjRule adjustment_rule_pay;
			oa::time::AdjRule adjustment_rule_fix;
			
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

			std::vector<oa::derived_time::CashflowStruct> fixed_cf_fix_pay_adj{
				{"2025-1-3","2025-7-3","2025-1-3","2025-7-3","2024-12-31","2025-7-7", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2025-7-3","2026-1-3","2025-7-3","2026-1-3","2025-7-1","2026-1-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-1-3","2026-7-3","2026-1-3","2026-7-3","2025-12-31","2026-7-6", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-7-3","2027-1-3","2026-7-3","2027-1-3","2026-7-1","2027-1-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-1-3","2027-7-3","2027-1-3","2027-7-3","2026-12-31","2027-7-6", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-7-3","2028-1-3","2027-7-3","2028-1-3","2027-7-1","2028-1-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-1-3","2028-7-3","2028-1-3","2028-7-3","2027-12-31","2028-7-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-7-3","2029-1-3","2028-7-3","2029-1-3","2028-6-30","2029-1-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-1-3","2029-7-3","2029-1-3","2029-7-3","2028-12-29","2029-7-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-1-3","2029-7-3","2030-1-3","2029-6-29","2030-1-7", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-1-3","2029-7-3","2030-1-3","2029-6-29","2030-1-7", 1000000.0, 1.0, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kPrincipal }

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

				for (auto& cf : fixed_cf_fix_pay_adj) {
					cf.days = day_count->DayCount(cf.start_date, cf.end_date);
					cf.day_count_fraction = day_count->YearFraction(cf.start_date, cf.end_date);
					cf.cashflow_amount = cf.notional * (cf.rate * cf.day_count_fraction);
				}
				fixed_cf_fix_pay_adj.at(10).cashflow_amount = notional;
			}
			virtual void TearDown() override
			{
			}
	};

	/// <summary>
	/// unit test for CreateFixedCashflows with forward date direction and no adjustments
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsFwdTest)
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

	/// <summary>
	/// unit test for CreateFixedCashflows with backward date direction and no adjustments
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsBkwdTest)
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

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsFwdTestWithPayFixAdj)
	{
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::Currency::kUSD,
			oa::derived_time::DateDirection::kForward,
			oa::derived_time::CashflowType::kFixed,
			oa::derived_time::ResetDirection::kAdvance,
			oa::derived_time::StubType::kNone,
			std::nullopt,
			std::nullopt,
			oa::derived_time::DateFormula(tenor_pay, adjustment_rule_pay, pay_calendar),
			oa::derived_time::DateFormula(tenor_fix, adjustment_rule_fix, fix_calendar)

		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_fix_pay_adj));
	}

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsBkwdTestWithPayFixAdj)
	{
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::Currency::kUSD,
			oa::derived_time::DateDirection::kBackward,
			oa::derived_time::CashflowType::kFixed,
			oa::derived_time::ResetDirection::kAdvance,
			oa::derived_time::StubType::kNone,
			std::nullopt,
			std::nullopt,
			oa::derived_time::DateFormula(tenor_pay, adjustment_rule_pay ,pay_calendar),
			oa::derived_time::DateFormula(tenor_fix, adjustment_rule_fix ,fix_calendar)

		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_fix_pay_adj));
	}
}
