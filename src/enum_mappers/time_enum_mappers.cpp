#include "time_enum_mappers.h"


namespace oa::enum_mappers
{
	oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str)
	{
		std::string key_str = input_str;
		std::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);

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