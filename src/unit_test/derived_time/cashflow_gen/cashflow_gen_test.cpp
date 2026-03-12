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
				  biz_pay_days(2),
				  biz_fix_days(-2),
				  adjustment_rule_pay(oa::time::AdjRule::kModifiedFollowing),
				  adjustment_rule_fix(oa::time::AdjRule::kPreceding)

			{
			}

		protected:
			oa::time::Date start_date;
			oa::time::Date mat_date;
			oa::time::Date stub_first_start_date;
			oa::time::Date stub_last_end_date;
			oa::time::Tenor frequency;
			double notional;
			double rate;
			oa::time::DayCountRule day_cnt_rule;
			oa::derived_time::BusinessDateFormula date_formula_fix, date_formula_pay;
			std::string pay_calendar;
			std::string fix_calendar;
			int biz_pay_days;
			int biz_fix_days;
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


			std::vector<oa::derived_time::CashflowStruct> fixed_cf_base_short_first{
				{"2024-10-3","2025-1-3","2024-10-3","2025-1-3","2024-10-3","2025-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
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

			std::vector<oa::derived_time::CashflowStruct> fixed_cf_base_long_first{
				{"2024-10-3","2025-7-3","2024-10-3","2025-7-3","2024-10-3","2025-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
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

			std::vector<oa::derived_time::CashflowStruct> fixed_cf_base_short_last{
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
				{"2030-1-3","2030-5-3","2030-1-3","2030-5-3","2030-1-3","2030-5-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2030-1-3","2030-5-3","2030-1-3","2030-5-3","2030-1-3","2030-5-3", 1000000.0, 1.0, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kPrincipal },

			};

			std::vector<oa::derived_time::CashflowStruct> fixed_cf_base_long_last{
				{"2025-1-3","2025-7-3","2025-1-3","2025-7-3","2025-1-3","2025-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2025-7-3","2026-1-3","2025-7-3","2026-1-3","2025-7-3","2026-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-1-3","2026-7-3","2026-1-3","2026-7-3","2026-1-3","2026-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-7-3","2027-1-3","2026-7-3","2027-1-3","2026-7-3","2027-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-1-3","2027-7-3","2027-1-3","2027-7-3","2027-1-3","2027-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-7-3","2028-1-3","2027-7-3","2028-1-3","2027-7-3","2028-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-1-3","2028-7-3","2028-1-3","2028-7-3","2028-1-3","2028-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-7-3","2029-1-3","2028-7-3","2029-1-3","2028-7-3","2029-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-1-3","2029-7-3","2029-1-3","2029-7-3","2029-1-3","2029-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-5-3","2029-7-3","2030-5-3","2029-7-3","2030-5-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-5-3","2029-7-3","2030-5-3","2029-7-3","2030-5-3", 1000000.0, 1.0, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kPrincipal }

			};

			std::vector<oa::derived_time::CashflowStruct> fixed_cf_fix_pay_adj{
				{"2025-1-3","2025-7-3","2025-1-3","2025-7-3","2025-1-3","2025-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2025-7-3","2026-1-3","2025-7-3","2026-1-3","2025-7-3","2026-1-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-1-3","2026-7-3","2026-1-3","2026-7-3","2026-1-3","2026-7-3", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed},
				{"2026-7-3","2027-1-3","2026-7-3","2027-1-3","2026-7-1","2027-1-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-1-3","2027-7-3","2027-1-3","2027-7-3","2026-12-30","2027-7-7", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2027-7-3","2028-1-3","2027-7-3","2028-1-3","2027-7-1","2028-1-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-1-3","2028-7-3","2028-1-3","2028-7-3","2027-12-30","2028-7-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2028-7-3","2029-1-3","2028-7-3","2029-1-3","2028-6-29","2029-1-5", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-1-3","2029-7-3","2029-1-3","2029-7-3","2028-12-29","2029-7-6", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-1-3","2029-7-3","2030-1-3","2029-6-29","2030-1-7", 1000000.0, .05, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kFixed },
				{"2029-7-3","2030-1-3","2029-7-3","2030-1-3","2029-6-29","2030-1-7", 1000000.0, 1.0, 0.0, 0, 0, 0.0, dt::Currency::kUSD, dt::CashflowType::kPrincipal }

			};

			static void PopulateFixedCashflows(std::vector<oa::derived_time::CashflowStruct>& cash_flows, oa::time::DayCountRule day_cnt_rule,
				std::optional<oa::derived_time::BusinessDateFormula>  date_formula_pay = {},
				std::optional<oa::derived_time::BusinessDateFormula> date_formula_fix = {})
				{
					auto day_count = oa::time::DayCounterFactory::GenerateDayCounter(day_cnt_rule);
					for (auto& cf : cash_flows) {
						cf.days = day_count->DayCount(cf.start_date, cf.end_date);
						cf.day_count_fraction = day_count->YearFraction(cf.start_date, cf.end_date);
						cf.cashflow_amount = cf.notional * (cf.rate * cf.day_count_fraction);
						if (date_formula_pay.has_value())
						{
							cf.payment_date = date_formula_pay.value().Adjust(cf.end_date);
						}
						if (date_formula_fix.has_value())
						{
							cf.fixing_date = date_formula_fix.value().Adjust(cf.start_date);
						}
					}
					cash_flows.back().cashflow_amount = cash_flows.back().notional; //principal repayment
				}

			virtual void SetUp() override
			{
				stub_first_start_date = oa::time::Date(2024, 10, 3);
				stub_last_end_date = oa::time::Date(2030, 5, 3);
				start_date = oa::time::Date(2025, 1, 3);
				mat_date = oa::time::Date(2030, 1, 3);
				frequency = oa::time::Tenor("6M");
				notional = 1000000.0;
				rate = 0.05; //5%
				day_cnt_rule = oa::time::DayCountRule::kACT_360;

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
		PopulateFixedCashflows(fixed_cf_base, day_cnt_rule);
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kForward)
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
		PopulateFixedCashflows(fixed_cf_base, day_cnt_rule);
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kBackward)
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_base));
	}

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsBckwdTestShortFirst)
	{
		PopulateFixedCashflows(fixed_cf_base_short_first, day_cnt_rule);
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			stub_first_start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kBackward)
				.stub_type(oa::derived_time::StubType::kShortFirst)
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_base_short_first));
	}

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsBckwdTestlongFirst)
	{
		PopulateFixedCashflows(fixed_cf_base_long_first, day_cnt_rule);
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			stub_first_start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kBackward)
				.stub_type(oa::derived_time::StubType::kLongFirst)
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_base_long_first));
	}

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsFwdTestShortLast)
	{
		PopulateFixedCashflows(fixed_cf_base_short_last, day_cnt_rule);

		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			stub_last_end_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kForward)
				.stub_type(oa::derived_time::StubType::kShortLast)
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_base_short_last));
	}

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsFwdTestLongLast)
	{
		PopulateFixedCashflows(fixed_cf_base_long_last, day_cnt_rule);

		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			stub_last_end_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kForward)
				.stub_type(oa::derived_time::StubType::kLongLast)
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_base_long_last));
	}

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsFwdTestWithPayFixAdj)
	{
		PopulateFixedCashflows(fixed_cf_fix_pay_adj, day_cnt_rule,
			oa::derived_time::BusinessDateFormula(biz_pay_days, pay_calendar),
			oa::derived_time::BusinessDateFormula(biz_fix_days, fix_calendar));

		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kForward)
				.pay_adjustment({biz_pay_days, pay_calendar})
				.fix_adjustment({biz_fix_days, fix_calendar})
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_fix_pay_adj));
	}

	TEST_F(CashflowGenBaseTest, CreateFixedCashflowsBkwdTestWithPayFixAdj)
	{
		PopulateFixedCashflows(fixed_cf_fix_pay_adj, day_cnt_rule,
			oa::derived_time::BusinessDateFormula(biz_pay_days, pay_calendar),
			oa::derived_time::BusinessDateFormula(biz_fix_days, fix_calendar));
		auto cashflows = oa::derived_time::CashflowGen::CreateFixedCashflows(
			start_date,
			mat_date,
			oa::derived_time::Frequency::kSemiAnnual,
			notional,
			rate,
			day_cnt_rule,
			oa::derived_time::CashflowGen::Options{}
				.date_direction(oa::derived_time::DateDirection::kBackward)
				.pay_adjustment({biz_pay_days, pay_calendar})
				.fix_adjustment({biz_fix_days, fix_calendar})
		);
		EXPECT_THAT(cashflows, ::testing::Pointwise(::testing::Eq(), fixed_cf_fix_pay_adj));
	}

	TEST_F(CashflowGenBaseTest, TestExceptionThrow) {
		EXPECT_ANY_THROW(
			oa::derived_time::CashflowGen::CreateFixedCashflows(
				start_date,
				oa::time::Date(2020, 1, 3),
				oa::derived_time::Frequency::kSemiAnnual,
				notional,
				rate,
				day_cnt_rule,
				oa::derived_time::CashflowGen::Options{}
					.date_direction(oa::derived_time::DateDirection::kForward)
					.stub_type(oa::derived_time::StubType::kShortFirst)
					.pay_adjustment({biz_pay_days, pay_calendar})
					.fix_adjustment({biz_fix_days, fix_calendar})
			)
		);
	}
}  // namespace
