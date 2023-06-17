#include "day_count_30_360_bond_basis.h"

namespace oa::time
{
	int DayCount30360BondBasis::DayCount(const Date& date1, const Date& date2) const
	{
		return ComputeDayCount30360(date1, date2);
	}

	int DayCount30360BondBasis::ComputeDayCount30360(const Date& date1, const Date& date2)
	{
		if (date2 < date1)
		{
			return -ComputeDayCount30360(date2, date1);
		}
		
		int day1 = date1.m_days();
		int	day2 = date2.m_days();

		if (day1 == 31)
		{
			day1 = 30;
		}

		if (day2 == 31 && (day1 > 29))
		{
			day2 = 30;
		}

		return (date2.m_years() - date1.m_years()) * 360 + (date2.m_months() - date1.m_months())*30 + (day2 - day1);
	}

	double DayCount30360BondBasis::YearFraction(const Date& date1, const Date& date2) const
	{
		return ComputeDayCountFraction30360(date1, date2);

	}

	double DayCount30360BondBasis::ComputeDayCountFraction30360(const Date& date1, const Date& date2)
	{
		return (ComputeDayCount30360(date1, date2) / 360.0);
	}
}