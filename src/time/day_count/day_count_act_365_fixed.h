#ifndef ODINANALYTICS_TIME_DAYCOUNT_ACT_365_FIXED_H
#define ODINANALYTICS_TIME_DAYCOUNT_ACT_365_FIXED_H

#include "oa/dllexport.h"
#include "time/date.h"
#include "time/day_count/day_count_base.h"

namespace oa::time
{
	/// <summary>
	/// Class implements act/365 fixed daycount
	/// Defined by the 2006 ISDA definitions 4.16d
	/// </summary>
	class OA_TIME_API DayCountAct365Fixed : public DayCounterBase
	{
		public:
			DayCountAct365Fixed() = default;
			int DayCount(const Date& date1, const Date& date2) const override;
			double YearFraction(const Date& date1, const Date& date2) const override;

		private:
			static int ComputeDayCountAct365Fixed(const Date& date1, const Date& date2);
			static double ComputeDayCountFraction(const Date& date1, const Date& date2);

	};
}
#endif // !ODINANALYTICS_TIME_DAYCOUNT_ACT_365_FIXED_H



