#ifndef ODINANALYTICS_TIME_DAYCOUNT_ACT_ACT_H
#define ODINANALYTICS_TIME_DAYCOUNT_ACT_ACT_H

#include "oa/dllexport.h"
#include "time/day_count/day_count_base.h"

namespace oa::time
{
	/// <summary>
	/// this class implements the daycount convention
	/// act/act day count convention defined in 2006 ISDA definitions 4.16b
	/// </summary>
	class OA_TIME_API DayCountActAct : public DayCounterBase
	{
		public:
			DayCountActAct() = default;
			int DayCount(const Date& start_date, const Date& end_date) const override;
			double YearFraction(const Date& start_date, const Date& end_date) const override;

		private:
			static int ComputeDayCountActAct(const Date& start_date, const Date& end_date);
			static double ComputeDayCountFractionActAct(const Date& start_date, const Date& end_date);

	};
}

#endif // !ODINANALYTICS_TIME_DAYCOUNT_ACT_ACT_H






