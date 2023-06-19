#include <algorithm>
#include <format>
#include <fstream>
#include <string>
#include <unordered_map>

#include "time/time_enums.h"

#ifndef OPENANALYTICS_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
#define OPENANALYTICS_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_

namespace oa::enum_mappers {

	/**
	 * Return the string to `DayCountRule` enum map.
	 */
	const auto& TimeEnumMap();

	/// <summary>
	/// returns an DayCountRule enum from a given string
	/// </summary>
	/// <param name="input_str"></param>
	/// <returns></returns>
	time::DayCountRule MapInputToDayCountEnum(const std::string& input_str);

}  // namespace oa::enum_mappers

#endif // !OPENANALYTICS_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
