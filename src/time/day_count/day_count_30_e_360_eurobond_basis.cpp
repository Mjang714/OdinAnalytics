#include "day_count_30_e_360_eurobond_basis.h"


namespace oa::time
{
	int DayCount30E360EuroBondBasis::DayCount(const Date& date1, const Date& date2) const
	{
		return ComputeDayCount30E360(date1, date2);
	}

	int DayCount30E360EuroBondBasis::ComputeDayCount30E360(const Date& date1, const Date& date2)
	{
		// Gregorian dates
		auto [y1, m1, d1] = date1.gregorian();
		auto [y2, m2, d2] = date2.gregorian();

		if (d1 == 31)
		{
			d1 = 30;
		}

		if (d2 == 31)
		{
			d2 = 30;
		}

		return (360 * (y2 - y1)) + 30 * (m2 - m1) + (d2 - d1);
	}

	double DayCount30E360EuroBondBasis::YearFraction(const Date& date1, const Date& date2) const
	{
		return ComputeDayCountFraction30E360(date1, date2);
	}

	double DayCount30E360EuroBondBasis::ComputeDayCountFraction30E360(const Date& date1, const Date& date2)
	{
		return ComputeDayCount30E360(date1, date2) / 360.0;
	}
}