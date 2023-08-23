#ifndef ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTPLAIN_H
#define ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTPLAIN_H

#include "date_adjust_base.h"

#include "time/calendar.h"

namespace oa::time
{
	class DateAdjustPlain : public DateAdjustBase
	{
		public:
			DateAdjustPlain() = default;
			Date AdjustDate(const Date& given_date) const override { return given_date;}
	};
}
#endif // ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTPLAIN_H
