#include "day_counter_factory.h"

#include <memory>

#include "time/day_count/day_count_30_360e_isda.h"
#include "time/day_count/day_count_30_360_bond_basis.h"
#include "time/day_count/day_count_30_e_360_eurobond_basis.h"
#include "time/day_count/day_count_act_360.h"
#include "time/day_count/day_count_act_365_fixed.h"
#include "time/day_count/day_count_act_act.h"

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
			default:
				return nullptr;
		}

	}
}