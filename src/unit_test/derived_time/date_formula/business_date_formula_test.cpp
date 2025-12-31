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
			oa::time::Date start_date{"2026-11-10"};
			oa::time::Date end_date{ "2026-11-18" };
			oa::derived_time::BusinessDateFormula business_days_fwd{5, calendar};
			oa::derived_time::BusinessDateFormula business_days_bkwd{-5, calendar};
			oa::derived_time::BusinessDateFormula business_days_zero{0, calendar};
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

	TEST_F(BusinessDateFormulaTest, OperatorOverlaodingFwd)
	{
		EXPECT_EQ(end_date, business_days_fwd + start_date);
		EXPECT_EQ(end_date, start_date + business_days_fwd);
	}

	TEST_F(BusinessDateFormulaTest, OperatorOverlaodingBckwd)
	{
		EXPECT_EQ(start_date, business_days_bkwd + end_date);
		EXPECT_EQ(start_date, end_date + business_days_bkwd);
	}
}