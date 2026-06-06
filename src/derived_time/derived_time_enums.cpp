#include "derived_time_enums.h"

#include <algorithm>
#include <ostream>
#include <unordered_map>
#include "helpers/utils.h"

namespace oa::derived_time
{

    std::ostream& operator<<(std::ostream& os, DateDirection val)
	{
		return os << utils::GetCleanName(val);
	}

	std::ostream& operator<<(std::ostream& os, Frequency val)
	{
		return os << utils::GetCleanName(val);
	}

	std::ostream& operator<<(std::ostream& os, StubType val)
	{
		return os << utils::GetCleanName(val);
	}

	std::ostream& operator<<(std::ostream& os, CalcType val)
	{
		return os << utils::GetCleanName(val);
	}
}
