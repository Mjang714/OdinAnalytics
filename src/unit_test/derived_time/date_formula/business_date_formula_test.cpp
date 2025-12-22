#include <gtest/gtest.h>

#include "oa/testing/gtest.h"
#include "time/date.h"
#include "time/calendar.h"
#include "time/tenor.h"
#include "static_data_cache/calendar_cache.h"
#include "derived_time/date_formula/business_date_formula.h"

namespace
{
	class BusinessDateFormulaTest :public ::testing::Test
	{
		protected:
			std::string calendar{ "NYB" };
			oa::derived_time::BusinessDateFormula business_days_fwd;
			oa::derived_time::BusinessDateFormula  business_days_bkwd;
			oa::derived_time::BusinessDateFormula business_days_zero;
			oa::time::Date start_date{"2026-11-10"};
			oa::time::Date end_date{ "2026-11-18" };
			void SetUp() override
			{
				business_days_fwd = oa::derived_time::BusinessDateFormula(5, calendar);
				business_days_bkwd = oa::derived_time::BusinessDateFormula(-5, calendar);
				business_days_zero = oa::derived_time::BusinessDateFormula(0, calendar);
			}
	};

	TEST_F(BusinessDateFormulaTest, BusDateFormulaFwdTest)
	{
		EXPECT_EQ(end_date, business_days_fwd.Adjust(start_date));
	}

	TEST_F(BusinessDateFormulaTest, BusDateFormulaBckwdTest)
	{
		EXPECT_EQ(start_date, business_days_bkwd.Adjust(end_date));
	}

	TEST_F(BusinessDateFormulaTest, BusDateFormulaNoneTest)
	{
		EXPECT_EQ(start_date, business_days_zero.Adjust(start_date));
		EXPECT_EQ(end_date, business_days_zero.Adjust(end_date));
	}
}