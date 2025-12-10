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
		return end_date.GetJulian() - start_date.GetJulian();
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

		double days_in_start_year = Date::IsLeap(start_year) ? 366.0 : 365.0;
		double days_in_end_year = Date::IsLeap(end_year) ? 366.0 : 365.0;

		auto year_fraction = 0.0;
		if (end_year == start_year)
		{
			year_fraction += static_cast<double> (ComputeDayCountActAct(start_date, end_date) / days_in_start_year);
		}

		else 
		{
			auto curr_year = start_year;
			auto prior_date = start_date;
			while (curr_year < end_year)
			{
				auto days_in_curr_year = Date::IsLeap(curr_year) ? 366.0 : 365.0;
				auto curr_date = Date(curr_year, 12, 31);
				if(curr_date < end_date)
				{
					auto temp1 = ComputeDayCountActAct(prior_date, curr_date);
					year_fraction += static_cast<double> ((ComputeDayCountActAct(prior_date, curr_date) + 1) / days_in_curr_year);
					prior_date = curr_date;
				}
				curr_year++;
			}
			auto temp2 = ComputeDayCountActAct(Date(end_year, 1, 1), end_date);
			year_fraction += static_cast<double> (ComputeDayCountActAct(Date(end_year, 1, 1), end_date) / days_in_end_year);	
		}

		return year_fraction;

	}
}	