
#include "time/date.h"
#include "time/day_count/day_count_base.h"
#include "time/day_count/day_count_act_act.h"
#include "time/day_count/day_count_act_365_fixed.h"
#include "time/day_count/day_count_30_360e_isda.h"
#include "time/day_count/day_count_30_e_360_eurobond_basis.h"
#include "time/day_count/day_count_act_360.h"

#ifndef ODINANLYTICS_TIME_DAYCOUNTERFACTORY_H
#define ODINANLYTICS_TIME_DAYCOUNTERFACTORY_H

namespace oa::time {
	
	class DayCounterFactory
	{
		public:
			static std::unique_ptr<DayCounterBase> GenerateDayCounter(DayCountRule day_count_rule);

	};
}
#endif ODINANLYTICS_TIME_DAYCOUNTERFACTORY_H




