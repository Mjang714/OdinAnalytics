#ifndef ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTMODIFIEDFOLLOWING_H
#define ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTMODIFIEDFOLLOWING_H

#include "date_adjust_base.h"

#include "oa/dllexport.h"
#include "time/calendar.h"

namespace oa::time
{
	class OA_TIME_API DateAdjustModFollowing : public DateAdjustBase
	{
		public:
			DateAdjustModFollowing() = default;
			explicit DateAdjustModFollowing(const Calendar& given_holiday);
			Date AdjustDate(const Date& given_date) const override;

		private:
			Calendar m_holidays_;

			/// <summary>
			/// return the next business day, unless it is the next month then it will return the previous bizday
			/// </summary>
			/// <param name="given_date">comes from the user given as a date</param>
			/// <param name="given_calendar">member variable oof the class passed to a static function</param>
			/// <returns></returns>
			static Date AdjustModFollowing(const Date& given_date, const Calendar& given_calendar);

	};
}

#endif // ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTMODIFIEDFOLLOWING_H
