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

OA_ENUM_MAPPERS_API
time::AdjRule MapInputToDayAdjustEnum(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::DateDirection MapInputToDateDir(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::Frequency MapInputToFreq(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::StubType MapInputToStub(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::CalcType MapInputToCalcType(const std::string& input_str);

OA_DERIVED_TIME_API
std::ostream& operator<<(std::ostream& os, time::DayCountRule val);

OA_DERIVED_TIME_API
std::ostream& operator<<(std::ostream& os, time::AdjRule val);

OA_DERIVED_TIME_API
std::ostream& operator<<(std::ostream& os, derived_time::DateDirection val);

OA_DERIVED_TIME_API
std::ostream& operator<<(std::ostream& os, derived_time::Frequency val);

OA_DERIVED_TIME_API
std::ostream& operator<<(std::ostream& os, derived_time::StubType val);

OA_DERIVED_TIME_API
std::ostream& operator<<(std::ostream& os, derived_time::CalcType val);

}  // namespace oa::enum_mappers

#endif // !OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
