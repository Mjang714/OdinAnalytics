#include <gtest/gtest.h>

#include "oa/testing/gtest.h"
#include "time/date.h"
#include "time/calendar.h"
#include "time/tenor.h"
#include "static_data_cache/calendar_cache.h"
#include "derived_time/date_formula/date_formula.h"

namespace
{
	class DateFormulaTest : public ::testing::Test
	{
	protected:
		std::shared_ptr<const oa::time::Calendar> calendar;
		oa::time::Tenor tenor{ "1Y" };
		oa::time::AdjRule adjustment_rule;
		oa::derived_time::DateFormula date_formula, date_formula_str;
		oa::time::Date start_date{ "2022-9-4" };
		oa::time::Date expected_date{"2023-9-5"};

			void SetUp() override
			{
				calendar = oa::static_cache::CalendarCache::RetrieveCache().GetCalendar("NYB");
				adjustment_rule = oa::time::AdjRule::kModifiedFollowing;
				date_formula = oa::derived_time::DateFormula(tenor, adjustment_rule, *calendar);
				date_formula_str = oa::derived_time::DateFormula("1Y", adjustment_rule, "NYB");
			}
	};

	TEST_F(DateFormulaTest, DateFormulaTestAdjustTenorInput)
	{
		EXPECT_EQ(expected_date, date_formula.Adjust(start_date));
	}

	TEST_F(DateFormulaTest, DateFormulaTestAdjustStrInput)
	{
		EXPECT_EQ(expected_date, date_formula_str.Adjust(start_date));
	}

	TEST_F(DateFormulaTest, DateFormulaTestoeratorOverloading)
	{
		EXPECT_EQ(expected_date, start_date + date_formula_str);
		EXPECT_EQ(expected_date, date_formula_str + start_date);
	}
}
