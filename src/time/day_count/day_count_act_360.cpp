#include "day_count_act_360.h"


namespace oa::time
{
	int DayCountAct360::DayCount(const Date& date1, const Date& date2) const
	{
		return ComputeDayCountAct360(date1, date2);
	}

	int DayCountAct360::ComputeDayCountAct360(const Date& date1, const Date& date2)
	{
		return date2.GetJulian() - date1.GetJulian();
	}

	double DayCountAct360::YearFraction(const Date& date1, const Date& date2) const
	{
		return ComputeDayCountFractionAct360(date1, date2);
	}

	double DayCountAct360::ComputeDayCountFractionAct360(const Date& date1, const Date& date2)
	{
		return ComputeDayCountAct360(date1, date2) / 360.0;
	}
}
