#ifndef ODINANALYTICS_TIME_DATEADJUST_ADJUSTMENTFACTORY_H
#define ODINANALYTICS_TIME_DATEADJUST_ADJUSTMENTFACTORY_H

#include <memory>

#include "oa/dllexport.h"
#include "time/calendar.h"
#include "time/date_adjust/date_adjust_base.h"
#include "time/time_enums.h"

namespace oa::time
{
	class OA_TIME_API AdjustmentFactory
	{
		public:
			static std::unique_ptr<DateAdjustBase> CreateDateAdjust(const AdjRule biz_day_convnetion, const Calendar& given_calendar);
			static std::unique_ptr<DateAdjustBase> CreateDateAdjust(const AdjRule biz_day_convnetion);
	};
}

#endif //ODINANALYTICS_TIME_DATEADJUST_ADJUSTMENTFACTORY_H
