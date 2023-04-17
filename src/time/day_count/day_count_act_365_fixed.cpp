#include "day_count_act_365_fixed.h"

namespace oa::time
{

	int DayCountAct365Fixed::DayCount(const Date& date1, const Date& date2) const
	{
		return ComputeDayCountAct365Fixed(date1, date2);
	}

	double DayCountAct365Fixed::YearFraction(const Date& date1, const Date& date2) const
	{
		return ComputeDayCountFraction(date1, date2);
	}

	int DayCountAct365Fixed::ComputeDayCountAct365Fixed(const Date& date1, const Date& date2)
	{
		return date2.GetJulian() - date1.GetJulian();
	}

	double DayCountAct365Fixed::ComputeDayCountFraction(const Date& date1, const Date& date2)
	{
		return ComputeDayCountAct365Fixed(date1, date2) / 365.0;
	}
}