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
						"Not a Valid day count convention please check input of: \"" + input_str + "\"",
						"time_enum_mappers.cpp line 100 MapInputToDayAdjustEnum()"
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
	
	const auto& DateDirectionMap() {
		static const std::unordered_map<std::string, derived_time::DateDirection> dd_map{
			{"FWD", derived_time::DateDirection::kForward},
			{"FORWARD", derived_time::DateDirection::kForward},
			{"BCKWD", derived_time::DateDirection::kBackward},
			{"BACKWARDS", derived_time::DateDirection::kBackward}
		};
		return dd_map;
	}

	//this is begining to smell and maybe we do some kind redesign 
	derived_time::DateDirection MapInputToDateDir(const std::string& input_str) {
		std::string key_str = input_str;
		// disable C4242, C4244 warnings about int being narrowed to char
		OA_MSVC_WARNING_PUSH()
		OA_MSVC_WARNING_DISABLE(4242 4244)
		std::ranges::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);
		OA_MSVC_WARNING_POP()

			if (DateDirectionMap().contains(key_str))
				return DateDirectionMap().at(key_str);
			else
			{
				throw std::invalid_argument{
			#if OA_HAS_CPP20_FORMAT
					std::format(
						"{}:{}",
						"Not a Valid Date Direction convention please check input of: \"" + input_str + "\"",
						"time_enum_mappers.cpp line 142 MapInputToDateDir()"
					)
			#else
					// __FILE__, __LINE__, __func__ let us avoid hardcoding
					std::string{__FILE__} + ":" + std::to_string(__LINE__) + ":" +
						std::string{__func__} + ": " + input_str +
						" is not a valid Date Direction convention"
			#endif  // !OA_HAS_CPP20_FORMAT
				};
			}
	}

	const auto& FreqMap(){
		static const std::unordered_map<std::string, derived_time::Frequency> freq_map {
			{"ONCE", derived_time::Frequency::kOnce},
			{"O", derived_time::Frequency::kOnce},
			{"A", derived_time::Frequency::kAnnual},
			{"ANNUAL", derived_time::Frequency::kAnnual},
			{"SA", derived_time::Frequency::kSemiAnnual},
			{"SEMIANNUAL", derived_time::Frequency::kSemiAnnual},
			{"Q", derived_time::Frequency::kQuarterly},
			{"QUARTERLY", derived_time::Frequency::kQuarterly},
			{"M", derived_time::Frequency::kMonthly},
			{"MONTHLY", derived_time::Frequency::kMonthly},
			{"W", derived_time::Frequency::kWeekly},
			{"WEEKLY", derived_time::Frequency::kWeekly},
			{"D", derived_time::Frequency::kDaily},
			{"DAILY", derived_time::Frequency::kDaily}
		};
		return freq_map;
	}

	//this is begining to smell and maybe we do some kind redesign 
	derived_time::Frequency MapInputToFreq(const std::string& input_str) {
		std::string key_str = input_str;
		// disable C4242, C4244 warnings about int being narrowed to char
		OA_MSVC_WARNING_PUSH()
		OA_MSVC_WARNING_DISABLE(4242 4244)
		std::ranges::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);
		OA_MSVC_WARNING_POP()

		if (FreqMap().contains(key_str))
			return FreqMap().at(key_str);
		else
		{
			throw std::invalid_argument{
		#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}:{}: {} is not a valid Frequency convention",
					__FILE__, __LINE__, __func__, input_str
				)
		#else
				// __FILE__, __LINE__, __func__ let us avoid hardcoding
				std::string{__FILE__} + ":" + std::to_string(__LINE__) + ":" +
					std::string{__func__} + ": " + input_str +
					" is not a valid Frequency convention"
		#endif  // !OA_HAS_CPP20_FORMAT
			};
		}
	}
	
	const auto& CFStubMap() {
		static const std::unordered_map<std::string, derived_time::StubType> stub_map {
			{"NONE", derived_time::StubType::kNone},
			{"N", derived_time::StubType::kNone},
			{"SF",  derived_time::StubType::kShortFirst},
			{"SHORTFIRST",  derived_time::StubType::kShortFirst},
			{"LF",  derived_time::StubType::kLongFirst},
			{"LONGFIRST",  derived_time::StubType::kLongFirst},
			{"SL",  derived_time::StubType::kShortLast},
			{"SHORTLAST",  derived_time::StubType::kShortLast},
			{"LL",  derived_time::StubType::kLongLast},
			{"LONGLAST",  derived_time::StubType::kLongLast},
		};
		return stub_map;
	}

	derived_time::StubType MapInputToStub(const std::string& input_str){
		std::string key_str = input_str;
		// disable C4242, C4244 warnings about int being narrowed to char
		OA_MSVC_WARNING_PUSH()
		OA_MSVC_WARNING_DISABLE(4242 4244)
		std::ranges::transform(input_str.begin(), input_str.end(), key_str.begin(), ::toupper);
		OA_MSVC_WARNING_POP()

		if (CFStubMap().contains(key_str))
			return CFStubMap().at(key_str);
		else
		{
			throw std::invalid_argument{
		#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}",
					"Not a Valid Stub Type convention please check input of: \"" + input_str + "\"",
					"time_enum_mappers.cpp line 237 MapInputToStub()"
				)
		#else
				// __FILE__, __LINE__, __func__ let us avoid hardcoding
				std::string{__FILE__} + ":" + std::to_string(__LINE__) + ":" +
					std::string{__func__} + ": " + input_str +
					" is not a valid Stub Type"
		#endif  // !OA_HAS_CPP20_FORMAT
			};
		}
	}
}  // namespace oa::enum_mappers
