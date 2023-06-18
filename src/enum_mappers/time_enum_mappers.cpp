#include "time_enum_mappers.h"

#include "helpers/warnings.h"

namespace oa::enum_mappers
{
	oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str)
	{
		std::string key_str = input_str;
		// disable C4242, C4244 warnings about int being narrowed to char
		OA_MSVC_WARNING_PUSH()
		OA_MSVC_WARNING_DISABLE(4242 4244)
		std::ranges::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);
		OA_MSVC_WARNING_POP()

		if (TimeEnumMap().day_count_mappers.contains(key_str))
		{
			return TimeEnumMap().day_count_mappers.at(key_str);
		}

		else
		{
			throw std::invalid_argument(
				std::format("{}:{}",
				"Not a Valid day count convention please check input of: "+  input_str,
					"time_enum_mappers.cpp line 19 MapInputToDayCountEnum()"));
		}

	}
}