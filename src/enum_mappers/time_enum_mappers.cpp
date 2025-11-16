#include "time_enum_mappers.h"

#include <algorithm>
#include <fstream>
#include <unordered_map>

#include "oa/platform.h"
#include "oa/warnings.h"

#if OA_HAS_CPP20_FORMAT
#include <format>
#endif  // !OA_HAS_CPP20_FORMAT

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
	const auto& DateAdjustMap()

	{
		static const std::unordered_map<std::string, time::AdjRule> da_map{
			{"MF", time::AdjRule::kModifiedFollowing},
			{"MODIFIEDFOLLOWING", time::AdjRule::kModifiedFollowing },
			{"MODIFIED_FOLLOWING", time::AdjRule::kModifiedFollowing },
			{"MODF", time::AdjRule::kModifiedFollowing },
			{"MFOL", time::AdjRule::kModifiedFollowing },
			{"P", time::AdjRule::kPreceding },
			{"PRECEDING", time::AdjRule::kPreceding },
			{"PRE", time::AdjRule::kPreceding },
			{"F", time::AdjRule::kFollowing },
			{"FOLLOWING", time::AdjRule::kFollowing },
			{"FOL", time::AdjRule::kFollowing },
			{ "NOADJ", time::AdjRule::kPlainAdjustment },
			{ "NO_ADJ", time::AdjRule::kPlainAdjustment },
			{ "NONE", time::AdjRule::kPlainAdjustment },
		};
		return da_map;
	}
	oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str)
	{
		std::string key_str = input_str;
// disable C4242, C4244 warnings about int being narrowed to char
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4242 4244)
		std::ranges::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);
OA_MSVC_WARNING_POP()

		if (TimeEnumMap().contains(key_str))
			return TimeEnumMap().at(key_str);
		else
		{
			throw std::invalid_argument{
#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}",
					"Not a Valid day count convention please check input of: " + input_str,
					"time_enum_mappers.cpp line 19 MapInputToDayCountEnum()"
				)
#else
				// __FILE__, __LINE__, __func__ let us avoid hardcoding
				std::string{__FILE__} + ":" + std::string{__LINE__} + ":" +
				std::string{__func__} + ": " + input_str +
				" is not a valid day count convention"
#endif  // !OA_HAS_CPP20_FORMAT
			};
		}

	}

	oa::time::AdjRule MapInputToDayAdjustEnum(const std::string& input_str)
	{
		std::string key_str = input_str;
		// disable C4242, C4244 warnings about int being narrowed to char
		OA_MSVC_WARNING_PUSH()
			OA_MSVC_WARNING_DISABLE(4242 4244)
			std::ranges::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);
		OA_MSVC_WARNING_POP()

			if (DateAdjustMap().contains(key_str))
				return DateAdjustMap().at(key_str);
			else
			{
				throw std::invalid_argument{
#if OA_HAS_CPP20_FORMAT
					std::format(
						"{}:{}",
						"Not a Valid day count convention please check input of: " + input_str,
						"time_enum_mappers.cpp line 115 MapInputToDayAdjustEnum()"
					)
#else
					// __FILE__, __LINE__, __func__ let us avoid hardcoding
					std::string{__FILE__} + ":" + std::string{__LINE__} + ":" +
						std::string{__func__} + ": " + input_str +
						" is not a valid day count convention"
#endif  // !OA_HAS_CPP20_FORMAT
				};
			}

	}

}  // namespace oa::enum_mappers
