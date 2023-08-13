#ifndef ODINANALYTICS_TIME_DAYCOUNT_30_360E_ISDA_H
#define ODINANALYTICS_TIME_DAYCOUNT_30_360E_ISDA_H

#include "day_count_base.h"

#include "oa/dllexport.h"
#include "time/date.h"

namespace oa::time
{
	/// <summary>
	/// Class implements the 30E/360 (ISDA)
	/// from 2006 ISDA Definitions Sec 4.16 (h)
	/// </summary>
	class OA_TIME_API DayCount30360EISDA : public DayCounterBase
	{
		public:
			DayCount30360EISDA() = default;
			int DayCount(const Date& date1, const Date& date2) const override;
			double YearFraction(const Date& date1, const Date& date2) const override;

		private:
			static int ComputeDayCount30360EISDA(const Date& date1, const Date& date2);
			static double ComputeDayCountFraction30360EISDA(const Date& date1, const Date& date2);

	};
}


#endif //ODINANALYTICS_TIME_DAYCOUNT_30_360E_ISDA_H