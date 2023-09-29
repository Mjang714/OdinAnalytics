#include "gtest/gtest.h"

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
			oa::time::Tenor tenor{"1Y"};
			oa::time::AdjRule adjustment_rule;
			oa::derived_time::DateFormula date_formula;
			virtual void SetUp() override
			{
				calendar = oa::static_cache::CalendarCache::RetrieveCache().GetCalendar("NYB");
				adjustment_rule = oa::time::AdjRule::kModifiedFollowing;
				date_formula = oa::derived_time::DateFormula(tenor, adjustment_rule, *calendar);
			}

			virtual void TearDown() override
			{

			}
	};

	TEST_F(DateFormulaTest, DateFormulaTestAdjust)
	{
		oa::time::Date start_date(2022,9,4);
		oa::time::Date resulting_date;
		oa::time::Date expected_date(2023, 9, 5);

		resulting_date = date_formula.Adjust(start_date);

		EXPECT_EQ(expected_date.GetJulian(), resulting_date.GetJulian());
	}
}