#ifndef ODINANALYTICS_TIME_DAYCOUNT_ACT_360_H
#define ODINANALYTICS_TIME_DAYCOUNT_ACT_360_H

#include "oa/dllexport.h"
#include "time/date.h"
#include "time/day_count/day_count_base.h"

namespace oa::time
{
	/// <summary>
	///
	/// </summary>
	class OA_TIME_API DayCountAct360: public DayCounterBase
	{
		public:
			DayCountAct360() = default;
			int DayCount(const Date& date1, const Date& date2) const override;
			double YearFraction(const Date& date1, const Date& date2) const override;

		private:
			static int ComputeDayCountAct360(const Date& date1, const Date& date2);
			static double ComputeDayCountFractionAct360(const Date& date1, const Date& date2);

	};
}


#endif // !ODINANALYTICS_TIME_DAYCOUNT_ACT_360_H



