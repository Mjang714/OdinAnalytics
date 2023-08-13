#ifndef ODINANALYTICS_TIME_DAYCOUNTBASE_H
#define ODINANALYTICS_TIME_DAYCOUNTBASE_H

#include "oa/dllexport.h"
#include "time/date.h"

namespace oa::time
{
	/// <summary>
	/// the base class where all day count conventions are derived from
	/// sources for these day counts are
	/// https://strata.opengamma.io/apidocs/com/opengamma/strata/basics/date/DayCounts.html#:~:text=final%20DayCount%20ACT_ACT_ISDA-,The%20'Act%2FAct%20ISDA'%20day%20count%2C%20which%20divides,year%20is%20divided%20by%20366.
	///
	/// </summary>
	class OA_TIME_API DayCounterBase
	{
		public:

			virtual ~DayCounterBase() = default;

			/// <summary>
			/// takes in 2 days and figures out how many days are in between.
			/// </summary>
			/// <param name="Date1">first date</param>
			/// <param name="Date2">second date</param>
			/// <returns>integer of the number of da</returns>
			virtual int DayCount(const Date& date1, const Date& date2) const = 0 ;

			/// <summary>
			/// takes in 2 dates and figures out length of time in years
			/// </summary>
			/// <param name="Date1">first date</param>
			/// <param name="Date2">second date</param>
			/// <returns>double that represents the time in years</returns>
			virtual double YearFraction(const Date& date1, const Date& date2) const = 0;
	};
}

#endif //ODINANALYTICS_TIME_DAYCOUNTBASE_H
