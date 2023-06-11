#include <algorithm>
#include <fstream>
#include <format>
#include <string>
#include <unordered_map>

#include "time/time_enums.h"

#ifndef OPENANALYTICS_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
#define OPENANALYTICS_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_


namespace oa::enum_mappers
{
	struct TimeEnumMap
	{
		const std::unordered_map<std::string, oa::time::DayCountRule> day_count_mappers
		{
			{"ACT/360", oa::time::DayCountRule::kACT_360},
			{"ACT:360", oa::time::DayCountRule::kACT_360},
			{"ACT_360", oa::time::DayCountRule::kACT_360},
			{"ACT360", oa::time::DayCountRule::kACT_360},
			{"30E/360", oa::time::DayCountRule::k30_E_360_ISDA},
			{"30E:360", oa::time::DayCountRule::k30_E_360_ISDA},
			{"30E_360", oa::time::DayCountRule::k30_E_360_ISDA},
			{"30E360", oa::time::DayCountRule::k30_E_360_ISDA },
			{"30/360", oa::time::DayCountRule::k30_360_BOND_BASIS},
			{"30:360", oa::time::DayCountRule::k30_360_BOND_BASIS},
			{"30_360", oa::time::DayCountRule::k30_360_BOND_BASIS},
			{"30360", oa::time::DayCountRule::k30_360_BOND_BASIS},
			{"30E/360EURO", oa::time::DayCountRule::k30_360_E_EUROBOND},
			{"30E:360EURO", oa::time::DayCountRule::k30_360_E_EUROBOND},
			{"30E_360EURO", oa::time::DayCountRule::k30_360_E_EUROBOND},
			{"30E360EURO", oa::time::DayCountRule::k30_360_E_EUROBOND},
			{"ACT/ACT", oa::time::DayCountRule::kACT_ACT},
			{"ACT:ACT", oa::time::DayCountRule::kACT_ACT},
			{"ACT_ACT", oa::time::DayCountRule::kACT_ACT},
			{"ACTACT", oa::time::DayCountRule::kACT_ACT},
		};
	};
	/// <summary>
	/// returns an DayCountRule enum from a given string
	/// </summary>
	/// <param name="input_str"></param>
	/// <returns></returns>
	oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str);
}
#endif // !OPENANALYTICS_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
