#include "oa/time/enums.h"

#include <ostream>

#include "helpers/utils.h"

namespace oa::time
{
    std::ostream& operator<<(std::ostream& os, DayCountRule val)
	{
		return os << utils::GetCleanName(val);
	}

	std::ostream& operator<<(std::ostream& os, AdjRule val)
	{
		return os << utils::GetCleanName(val);
	}
}
