#ifndef ODINANALYTICS_TIME_DAYCOUNT_30_360_BOND_BASIS_H
#define ODINANALYTICS_TIME_DAYCOUNT_30_360_BOND_BASIS_H

#include "day_count_base.h"
#include "time/date.h"

namespace oa::time
{
	/// <summary>
	/// this class implements 30/360 (Bond Basis) described in 2006 ISDA Definitions, Sec 4.16 (f)
	/// note this verions is not the same as the one used by excel
	/// </summary>
	class DayCount30360BondBasis : public DayCounterBase
	{
		public:
			DayCount30360BondBasis() = default;
			int DayCount(const Date& date1, const Date& date2) const override;
			double YearFraction(const Date& date1, const Date& date2) const override;

		private:
			static int ComputeDayCount30360(const Date& date1, const Date& date2);
			static double ComputeDayCountFraction30360(const Date& date1, const Date& date2);
	};
}


#endif //ODINANALYTICS_TIME_DAYCOUNT_30_360_BOND_BASIS_H