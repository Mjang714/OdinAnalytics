#ifndef ODINANALYTICS_TIME_DATEADJUST_ADJUSTMENTFACTORY_H
#define ODINANALYTICS_TIME_DATEADJUST_ADJUSTMENTFACTORY_H

#include "time/calendar.h"
#include "time/time_enums.h"
#include "date_adjust_base.h"
#include "date_adjust_preceding.h"
#include "date_adjust_following.h"
#include "date_adjust_plain.h"
#include "date_adjust_modified_following.h"

namespace oa::time
{
	class AdjustmentFactory
	{
		public:
			static std::unique_ptr<DateAdjustBase> CreateDateAdjust(const AdjRule biz_day_convnetion, const Calendar& given_calendar);
			static std::unique_ptr<DateAdjustBase> CreateDateAdjust(const AdjRule biz_day_convnetion);
	};
}

#endif //ODINANALYTICS_TIME_DATEADJUST_ADJUSTMENTFACTORY_H
