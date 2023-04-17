#include "gtest/gtest.h"

#include<vector>

#include "time/calendar_data_struct.h"
#include "time/calendar.h"
#include "time/date.h"
#include "time/time_enums.h"
#include "time/date_adjust/date_adjust_plain.h"

namespace
{
	oa::time::DateAdjustPlain date_adjust_plain_test;

	TEST(DateAdjustPlainTest, NoDateAdjust)
	{
		oa::time::Date valid_biz_day(2022, 11, 10);

		oa::time::Date adjusted_date = date_adjust_plain_test.AdjustDate(valid_biz_day);

		EXPECT_EQ(valid_biz_day, adjusted_date);
	}

	TEST(DateAdjustPlainTest, AdjustHoliday)
	{
		oa::time::Date invalid_biz_day(2022, 11, 12);

		oa::time::Date adjusted_date = date_adjust_plain_test.AdjustDate(invalid_biz_day);

		EXPECT_EQ(invalid_biz_day, adjusted_date);
	}
	

}