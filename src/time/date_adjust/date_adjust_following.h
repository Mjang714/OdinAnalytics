#ifndef ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTFOLLOWING_H
#define ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTFOLLOWING_H

#include "date_adjust_base.h"

#include "time/calendar.h"

namespace oa::time
{

	class DateAdjustFollowing : public DateAdjustBase
	{
		public:
			DateAdjustFollowing() = default;
			DateAdjustFollowing(const oa::time::Calendar& given_calendar);

			oa::time::Date AdjustDate(const oa::time::Date& given_date) const override;

		private:
			oa::time::Calendar m_holidays_;

			/// <summary>
			/// return the next Business day
			/// </summary>
			/// <param name="given_date">comes from the user given as a date</param>
			/// <param name="given_calendar">member variable oof the class passed to a static function</param>
			/// <returns></returns>
			static oa::time::Date AdjustFollowing(const oa::time::Date& given_date, const oa::time::Calendar& given_calendar);
	};

}

#endif //ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTFOLLOWING_H
