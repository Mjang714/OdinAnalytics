#include "time_enums.h"

#include "helpers/utils.h"

namespace oa::time
{
    std::ostream& operator<<(std::ostream& os, DayCountRule val)
	{
		return os << oa::utils::GetCleanName(val);
	}

	std::ostream& operator<<(std::ostream& os, AdjRule val)
	{
		return os << oa::utils::GetCleanName(val);
	}
}
