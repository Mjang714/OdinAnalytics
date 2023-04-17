#include "day_counter_factory.h"


namespace oa::time
{
	std::unique_ptr<DayCounterBase> DayCounterFactory::GenerateDayCounter(DayCountRule day_count_rule)
	{
		switch (day_count_rule)
		{
			case DayCountRule::kACT_360:
				std::make_unique<DayCountAct360>();
			case DayCountRule::kACT_365_FIXED:
				std::make_unique<DayCountAct365Fixed>();
			case DayCountRule::k30_360_E_EUROBOND:
				std::make_unique<DayCount30E360EuroBondBasis>();
			case DayCountRule::kACT_ACT:
				std::make_unique<DayCountActAct>();
			default:
				return nullptr;
		}

	}
}