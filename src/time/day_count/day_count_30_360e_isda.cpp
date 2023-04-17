#include "day_count_30_360e_isda.h"

namespace oa::time
{
	int DayCount30360EISDA::DayCount(const Date& date1, const Date& date2) const
	{
		return ComputeDayCount30360EISDA(date1, date2);
	}

	int DayCount30360EISDA::ComputeDayCount30360EISDA(const Date& date1, const Date& date2)
	{
		
		int day1 = date1.m_days();
		int day2 = date2.m_days();

		int month1 = date1.m_months();
		int month2 = date2.m_months();

		int year1 = date1.m_years();
		int year2 = date2.m_years();

		if (day1 == 31 || ((month1 == 2) && 
			(day1 == oa::time::Date::DaysInMonth(month1, year1))))
		{
			day1 = 30;
		}

		if ((day2 == 31)||((month2 == 2) && 
			(day2 == oa::time::Date::DaysInMonth(month2, year2))))
		{
			day2 = 30;
		}

		return 360*(year2 -year1) + 30*(month2- month1) + (day2 - day1);
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