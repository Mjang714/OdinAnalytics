#include "time/time_enum_mappers.h"

#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <iosfwd>
#include <string>

#include "helpers/utils.h"
#include "oa/platform.h"
#include "oa/warnings.h"
#include "oa/string.h"

#if OA_HAS_CPP20_FORMAT
#include <format>
#endif  // !OA_HAS_CPP20_FORMAT


namespace oa::time {


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
			{"ACT/365", time::DayCountRule::kACT_365_FIXED},
			{"ACT:365", time::DayCountRule::kACT_365_FIXED},
			{"ACT_365", time::DayCountRule::kACT_365_FIXED},
			{"ACT365", time::DayCountRule::kACT_365_FIXED},
			{"act/365", time::DayCountRule::kACT_365_FIXED},
			{"act:365", time::DayCountRule::kACT_365_FIXED},
			{"act_365", time::DayCountRule::kACT_365_FIXED},
			{"act365", time::DayCountRule::kACT_365_FIXED},
			{"ACT:ACT", time::DayCountRule::kACT_ACT},
			{"ACT_ACT", time::DayCountRule::kACT_ACT},
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
			{"MODIFIEDFOLLOWING", time::AdjRule::kModifiedFollowing},
			{"MODIFIED_FOLLOWING", time::AdjRule::kModifiedFollowing},
			{"MODF", time::AdjRule::kModifiedFollowing},
			{"MFOL", time::AdjRule::kModifiedFollowing},
			{"P", time::AdjRule::kPreceding},
			{"PRECEDING", time::AdjRule::kPreceding},
			{"PRE", time::AdjRule::kPreceding},
			{"F", time::AdjRule::kFollowing},
			{"FOLLOWING", time::AdjRule::kFollowing},
			{"FOL", time::AdjRule::kFollowing},
			{"NOADJ", time::AdjRule::kPlainAdjustment},
			{"NO_ADJ", time::AdjRule::kPlainAdjustment},
			{"NONE", time::AdjRule::kPlainAdjustment},
		};
		return da_map;
	}


	oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str)
	{
		std::string key_str = input_str;

		std::ranges::for_each(key_str, [](auto& c) {  c = oa::to_upper(c); });


		if (TimeEnumMap().contains(key_str))
			return TimeEnumMap().at(key_str);
		else
		{
			throw std::invalid_argument{
#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}:{}: {} is not a valid Stub Type",
					__FILE__, __LINE__, __func__, input_str
				)
#else
				// __FILE__, __LINE__, __func__ let us avoid hardcoding
				std::string{__FILE__} + ":" + std::to_string(__LINE__) + ":" +
				std::string{__func__} + ": " + input_str +
				" is not a valid day count convention"
#endif  // !OA_HAS_CPP20_FORMAT
			};
		}

	}

	oa::time::AdjRule MapInputToDayAdjustEnum(const std::string& input_str)
	{
		std::string key_str = input_str;
		
		std::ranges::for_each(key_str, [](auto& c) {  c = oa::to_upper(c); });
		if (DateAdjustMap().contains(key_str))
			return DateAdjustMap().at(key_str);
		else
		{
			throw std::invalid_argument{
#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}:{}: {} is not a valid Stub Type",
					__FILE__, __LINE__, __func__, input_str
				)
#else
				// __FILE__, __LINE__, __func__ let us avoid hardcoding
				std::string{__FILE__} + ":" + std::to_string(__LINE__) + ":" +
					std::string{__func__} + ": " + input_str +
					" is not a valid day count convention"
#endif  // !OA_HAS_CPP20_FORMAT
			};
		}

	}

	std::ostream& operator<<(std::ostream& os, DayCountRule val)
	{
		return os << oa::utils::GetCleanName(val);
	}

	std::ostream& operator<<(std::ostream& os, AdjRule val)
	{
		return os << oa::utils::GetCleanName(val);
	}

}