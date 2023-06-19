#include "time_enum_mappers.h"

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

	oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str)
	{
		std::string key_str = input_str;
		std::ranges::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);

		if (TimeEnumMap().contains(key_str))
			return TimeEnumMap().at(key_str);
		else
		{
			throw std::invalid_argument(
				std::format("{}:{}",
				"Not a Valid day count convention please check input of: "+  input_str,
					"time_enum_mappers.cpp line 19 MapInputToDayCountEnum()"));
		}

	}

}  // namespace oa::enum_mappers
