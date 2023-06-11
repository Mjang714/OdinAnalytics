#include "day_count_act_act.h"

//
namespace oa::time
{
	int DayCountActAct::DayCount(const Date& start_date, const Date& end_date) const
	{
		return ComputeDayCountActAct(start_date, end_date);
	}
	
	int DayCountActAct::ComputeDayCountActAct(const Date& start_date, const Date& end_date)
	{
		return start_date.GetJulian() - end_date.GetJulian();
	}

	double DayCountActAct::YearFraction(const Date& start_date, const Date& end_date) const
	{
		return ComputeDayCountFractionActAct(start_date, end_date);
	}

	double DayCountActAct::ComputeDayCountFractionActAct(const Date& start_date, const Date& end_date)
	{
		if (end_date < start_date)
		{
			return -ComputeDayCountFractionActAct(end_date, start_date);
		}

		int start_year = start_date.m_years();
		int end_year = end_date.m_years();

		int days_in_start_year = Date::IsLeap(start_year) ? 366 : 365;
		int days_in_end_year = Date::IsLeap(end_year) ? 366 : 365;

		auto year_fraction = static_cast<double> (end_year - start_year);
		year_fraction += static_cast<double> (ComputeDayCountActAct(start_date, Date(start_year, 12, 31)) / days_in_start_year);
		year_fraction += static_cast<double> (ComputeDayCountActAct(Date(end_year, 1, 1), end_date) / days_in_end_year);

		return year_fraction;

	}
}	