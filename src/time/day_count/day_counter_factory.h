#ifndef ODINANLYTICS_TIME_DAYCOUNTERFACTORY_H
#define ODINANLYTICS_TIME_DAYCOUNTERFACTORY_H

#include <memory>

#include "oa/dllexport.h"
#include "time/day_count/day_count_base.h"
#include "time/time_enums.h"

namespace oa::time {

	class OA_TIME_API DayCounterFactory
	{
		public:
			static std::unique_ptr<DayCounterBase> GenerateDayCounter(DayCountRule day_count_rule);

	};
}
#endif  // ODINANLYTICS_TIME_DAYCOUNTERFACTORY_H
