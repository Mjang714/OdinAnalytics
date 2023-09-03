#ifndef ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTPRECEDING_H
#define ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTPRECEDING_H

#include "date_adjust_base.h"
#include "oa/dllexport.h"
#include "time/calendar.h"

namespace oa::time
{
	class OA_TIME_API DateAdjustPreceding : public DateAdjustBase
	{
		public:
			DateAdjustPreceding() = default;
			explicit DateAdjustPreceding(const Calendar& given_calendar);
			Date AdjustDate(const Date& given_date) const override;

		private:
			Calendar m_holidays_;

			/// <summary>
			/// returns the previous biz day if it is holiday otherwise jsut return the date itself
			/// </summary>
			/// <param name="given_date">comes from the user given as a date</param>
			/// <param name="given_calendar">member variable oof the class passed to a static function</param>
			/// <returns></returns>
			static Date AdjustPreceding(const Date& given_date, const Calendar& given_calendar);
	};
}

#endif	//ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTPRECEDING_H
