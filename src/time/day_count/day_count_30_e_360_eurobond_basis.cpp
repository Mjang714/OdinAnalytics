#include "day_count_30_e_360_eurobond_basis.h"


namespace oa::time
{
	int DayCount30E360EuroBondBasis::DayCount(const Date& date1, const Date& date2) const
	{
		return ComputeDayCount30E360(date1, date2);
	}

	int DayCount30E360EuroBondBasis::ComputeDayCount30E360(const Date& date1, const Date& date2)
	{
		int day1 = date1.m_days();
		int day2 = date2.m_days();

		if (day1 == 31)
		{
			day1 = 30;
		}

		if (day2 == 31)
		{
			day2 = 30;
		}

		return (360 * (date2.m_years() - date1.m_years())) + 30 * (date2.m_months() - date1.m_months()) + (day2 - day1);
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