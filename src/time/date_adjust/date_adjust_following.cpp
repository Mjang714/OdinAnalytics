#include "date_adjust_following.h"


namespace oa::time
{
	DateAdjustFollowing::DateAdjustFollowing(const oa::time::Calendar& given_calendar) 
		: m_holidays_(given_calendar)
	{

	}

	oa::time::Date DateAdjustFollowing::AdjustDate(const oa::time::Date& given_date) const
	{
		return AdjustFollowing(given_date, m_holidays_);
	}

	oa::time::Date DateAdjustFollowing::AdjustFollowing(const oa::time::Date& given_date, const oa::time::Calendar& given_calendar)
	{
		if (given_calendar.IsHoliday(given_date))
		{
			return given_calendar.GetNextBusinessDay(given_date);
		}

		return given_date;
	}
}
