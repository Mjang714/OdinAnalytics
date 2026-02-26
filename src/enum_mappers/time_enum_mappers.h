#include <string>

#include "time/time_enums.h"
#include "derived_time/derived_time_enums.h"

#include "oa/dllexport.h"

#ifndef OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
#define OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_

namespace oa::enum_mappers {



/// <summary>
/// returns an DayCountRule enum from a given string
/// </summary>
/// <param name="input_str"></param>
/// <returns></returns>
OA_ENUM_MAPPERS_API
time::DayCountRule MapInputToDayCountEnum(const std::string& input_str);
/**
 * Return the string to `DayCountRule` enum map.
 */
const auto& TimeEnumMap();


OA_ENUM_MAPPERS_API
time::AdjRule MapInputToDayAdjustEnum(const std::string& input_str);
const auto& DateAdjustMap();


OA_ENUM_MAPPERS_API
derived_time::DateDirection MapInputToDateDir(const std::string& input_str);
const auto& DateDirectionMap();

OA_ENUM_MAPPERS_API
derived_time::Frequency MapInputToFreq(const std::string& input_str);
const auto& FreqMap();

OA_ENUM_MAPPERS_API
derived_time::StubType MapInputToStub(const std::string& input_str);
const auto& CFStubMap();

}  // namespace oa::enum_mappers

#endif // !OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
