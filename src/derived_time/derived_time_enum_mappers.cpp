#include "derived_time_enum_mappers.h"

#include <algorithm>
#include <ostream>
#include <unordered_map>

#include "helpers/utils.h"
#include "oa/platform.h"
#include "oa/warnings.h"
#include "oa/string.h"

#if OA_HAS_CPP20_FORMAT
#include <format>
#endif  // !OA_HAS_CPP20_FORMAT

namespace oa::derived_time
{
	const auto& DateDirectionMap() {
		static const std::unordered_map<std::string, derived_time::DateDirection> dd_map{
			{"FWD", derived_time::DateDirection::kForward},
			{"FORWARD", derived_time::DateDirection::kForward},
			{"BCKWD", derived_time::DateDirection::kBackward},
			{"BACKWARD", derived_time::DateDirection::kBackward},
			{"BACKWARDS", derived_time::DateDirection::kBackward}
		};
		return dd_map;
	}

	//this is begining to smell and maybe we do some kind redesign 
	derived_time::DateDirection MapInputToDateDir(const std::string& input_str) {
		std::string key_str = input_str;
		
		std::ranges::for_each(key_str, [](auto& c) {  c = oa::to_upper(c); });

		if (DateDirectionMap().contains(key_str))
			return DateDirectionMap().at(key_str);
		else
		{
			throw std::invalid_argument{
#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}:{}: {} is not a valid Date Direction convention",
					__FILE__, __LINE__, __func__, input_str
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
		
		std::ranges::for_each(key_str, [](auto& c) {  c = oa::to_upper(c); });

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
		
		std::ranges::for_each(key_str, [](auto& c) {  c = oa::to_upper(c); });

		if (CFStubMap().contains(key_str))
			return CFStubMap().at(key_str);
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
					" is not a valid Stub Type"
#endif  // !OA_HAS_CPP20_FORMAT
			};
		}
	}

	const auto& CalcTypeMap() {
		static const std::unordered_map<std::string, derived_time::CalcType> calc_type_map{
			{"FLAT", derived_time::CalcType::kFlat},
			{"BBG1", derived_time::CalcType::kBBGCalcType1},
			{"BBG2", derived_time::CalcType::kBBGCalcType2},
			{"USTSTREETCONV", derived_time::CalcType::kUSTStreetConv}
		};
		return calc_type_map;
	}

	derived_time::CalcType MapInputToCalcType(const std::string& input_str) {
		std::string key_str = input_str;
		std::ranges::for_each(key_str, [](auto& c) {  c = oa::to_upper(c); });
		if (CalcTypeMap().contains(key_str))
			return CalcTypeMap().at(key_str);
		else
		{
			throw std::invalid_argument{
#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}:{}: {} is not a valid Calc Type",
					__FILE__, __LINE__, __func__, input_str
				)
#else
				// __FILE__, __LINE__, __func__ let us avoid hardcoding
				std::string{__FILE__} + ":" + std::to_string(__LINE__) + ":" +
					std::string{__func__} + ": " + input_str +
					" is not a valid Calc Type"
#endif  // !OA_HAS_CPP20_FORMAT
			};
		}
	}
}
