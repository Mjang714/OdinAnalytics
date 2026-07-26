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

		// get Gregorian dates
		auto [y1, m1, d1] = date1.gregorian();
		auto [y2, m2, d2] = date2.gregorian();

		if (d1 == 31)
		{
			d1 = 30;
		}

		if (d2 == 31 && (d1 > 29))
		{
			d2 = 30;
		}

		return (y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1);
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