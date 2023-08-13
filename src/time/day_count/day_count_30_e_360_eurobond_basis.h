#ifndef ODINANALYTICS_TIME_DAYCOUNT_30_E_360_H
#define ODINANALYTICS_TIME_DAYCOUNT_30_E_360_H

#include "day_count_base.h"

#include "oa/dllexport.h"
#include "time/date.h"

namespace oa::time
{
	class OA_TIME_API DayCount30E360EuroBondBasis : public DayCounterBase
	{

		/// <summary>
		/// this class implements the daycount convetioonfor 30E/360 (EuroBond Basis) as described
		/// in ISDA 2006 Definitions 4.16 (g) Also Based on ICMA(rule 251) and FBF. This is the convention used
		/// Excel
		/// </summary>
		public:
			DayCount30E360EuroBondBasis() = default;
			int DayCount(const Date& date1, const Date& date2) const override;
			double YearFraction(const Date& date1, const Date& date2) const override;

		private:
			static int ComputeDayCount30E360(const Date& date1, const Date& date2);
			static double ComputeDayCountFraction30E360(const Date& date1, const Date& date2);
	};
}

#endif //ODINANALYTICS_TIME_DAYCOUNT_30_E_360_H
