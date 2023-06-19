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
	const auto& TimeEnumMap()
	{
		static const std::unordered_map<std::string, time::DayCountRule> dc_map{
			{"ACT/360", time::DayCountRule::kACT_360},
			{"ACT:360", time::DayCountRule::kACT_360},
			{"ACT_360", time::DayCountRule::kACT_360},
			{"ACT360", time::DayCountRule::kACT_360},
			{"30E/360", time::DayCountRule::k30_E_360_ISDA},
			{"30E:360", time::DayCountRule::k30_E_360_ISDA},
			{"30E_360", time::DayCountRule::k30_E_360_ISDA},
			{"30E360", time::DayCountRule::k30_E_360_ISDA },
			{"30/360", time::DayCountRule::k30_360_BOND_BASIS},
			{"30:360", time::DayCountRule::k30_360_BOND_BASIS},
			{"30_360", time::DayCountRule::k30_360_BOND_BASIS},
			{"30360", time::DayCountRule::k30_360_BOND_BASIS},
			{"30E/360EURO", time::DayCountRule::k30_360_E_EUROBOND},
			{"30E:360EURO", time::DayCountRule::k30_360_E_EUROBOND},
			{"30E_360EURO", time::DayCountRule::k30_360_E_EUROBOND},
			{"30E360EURO", time::DayCountRule::k30_360_E_EUROBOND},
			{"ACT/ACT", time::DayCountRule::kACT_ACT},
			{"ACT:ACT", time::DayCountRule::kACT_ACT},
			{"ACT_ACT", time::DayCountRule::kACT_ACT},
			{"ACTACT", time::DayCountRule::kACT_ACT},
		};
		return dc_map;
	}

	/// <summary>
	/// returns an DayCountRule enum from a given string
	/// </summary>
	/// <param name="input_str"></param>
	/// <returns></returns>
	time::DayCountRule MapInputToDayCountEnum(const std::string& input_str);

}  // namespace oa::enum_mappers

#endif // !OPENANALYTICS_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
