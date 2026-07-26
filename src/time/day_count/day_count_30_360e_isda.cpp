#include "day_count_30_360e_isda.h"

namespace oa::time
{
	int DayCount30360EISDA::DayCount(const Date& date1, const Date& date2) const
	{
		return ComputeDayCount30360EISDA(date1, date2);
	}

	int DayCount30360EISDA::ComputeDayCount30360EISDA(const Date& date1, const Date& date2)
	{
		// Gregorian dates
		auto [y1, m1, d1] = date1.gregorian();
		auto [y2, m2, d2] = date2.gregorian();

		if (d1 == 31 || ((m1 == 2) && (d1 == Date::DaysInMonth(y1, m1))))
		{
			d1 = 30;
		}

		if ((d2 == 31) || ((m2 == 2) && (d2 == Date::DaysInMonth(y2, m2))))
		{
			d2 = 30;
		}

		return 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
	}

	double DayCount30360EISDA::YearFraction(const Date& date1, const Date& date2) const
	{
		return ComputeDayCountFraction30360EISDA(date1, date2);
	}

	double DayCount30360EISDA::ComputeDayCountFraction30360EISDA(const Date& date1, const Date& date2)
	{
		return ComputeDayCount30360EISDA(date1, date2) / 360.0;
	}
}