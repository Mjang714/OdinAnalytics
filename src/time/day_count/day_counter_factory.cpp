#include "day_counter_factory.h"


namespace oa::time
{
	std::unique_ptr<DayCounterBase> DayCounterFactory::GenerateDayCounter(DayCountRule day_count_rule)
	{
		switch (day_count_rule)
		{
			case DayCountRule::kACT_360:
				return std::make_unique<DayCountAct360>();
			case DayCountRule::kACT_365_FIXED:
				return std::make_unique<DayCountAct365Fixed>();
			case DayCountRule::kACT_ACT:
				return std::make_unique<DayCountActAct>();
			case DayCountRule::k30_360_E_EUROBOND:
				return std::make_unique<DayCount30E360EuroBondBasis>();
			case DayCountRule::k30_E_360_ISDA:
				return std::make_unique<DayCount30360EISDA>();
			case DayCountRule::k30_360_BOND_BASIS:
				return std::make_unique<DayCount30360BondBasis>();
			// MSVC C4061 if we don't explicitly include a case
			case DayCountRule::kACTB_ACTB:
			default:
				return nullptr;
		}

	}
}