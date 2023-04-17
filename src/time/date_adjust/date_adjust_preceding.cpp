#include "date_adjust_preceding.h"

namespace oa::time
{
	DateAdjustPreceding::DateAdjustPreceding(const Calendar& given_calendar) : 
		m_holidays_(given_calendar)
	{

	}

	Date DateAdjustPreceding::AdjustDate(const Date& given_date) const
	{
		return AdjustPreceding(given_date, m_holidays_);
	}

	Date DateAdjustPreceding::AdjustPreceding(const Date& given_date, const Calendar& given_calendar)
	{
		if (given_calendar.IsHoliday(given_date))
		{
			return given_calendar.GetPrevBusinessDay(given_date);
		}

		return given_date;
	}

}

