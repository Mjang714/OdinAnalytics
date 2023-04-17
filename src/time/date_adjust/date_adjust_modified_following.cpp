#include "date_adjust_modified_following.h"

namespace oa::time
{
	DateAdjustModFollowing::DateAdjustModFollowing(const Calendar& given_calendar) 
		: m_holidays_(given_calendar)
	{

	}

	Date DateAdjustModFollowing::AdjustDate(const Date& given_date) const
	{
		return AdjustModFollowing(given_date, m_holidays_);
	}

	Date DateAdjustModFollowing::AdjustModFollowing(const Date& given_date, const Calendar& given_calendar)
	{
		
		if (given_calendar.IsHoliday(given_date))
		{
			Date next_business_day = given_calendar.GetNextBusinessDay(given_date);
			if (given_date.m_months() != next_business_day.m_months())
			{
				return given_calendar.GetPrevBusinessDay(given_date);
			}

			return next_business_day;

		}
		return given_date;
	}
}