// time_enum_mappers.h

#ifndef OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
#define OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_

#include <iosfwd>
#include <string>

#include "time/time_enums.h"
#include "derived_time/derived_time_enums.h"

#include "oa/dllexport.h"

namespace oa {

namespace enum_mappers {

/// <summary>
/// returns an DayCountRule enum from a given string
/// </summary>
/// <param name="input_str"></param>
/// <returns></returns>
// OA_ENUM_MAPPERS_API
// time::DayCountRule MapInputToDayCountEnum(const std::string& input_str);

// OA_ENUM_MAPPERS_API
// time::AdjRule MapInputToDayAdjustEnum(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::DateDirection MapInputToDateDir(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::Frequency MapInputToFreq(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::StubType MapInputToStub(const std::string& input_str);

OA_ENUM_MAPPERS_API
derived_time::CalcType MapInputToCalcType(const std::string& input_str);

}  // namespace enum_mappers

// TODO: consider declaring operator<< near related enums instead

namespace derived_time {

OA_ENUM_MAPPERS_API
std::ostream& operator<<(std::ostream& os, DateDirection val);

OA_ENUM_MAPPERS_API
std::ostream& operator<<(std::ostream& os, Frequency val);

OA_ENUM_MAPPERS_API
std::ostream& operator<<(std::ostream& os, StubType val);

OA_ENUM_MAPPERS_API
std::ostream& operator<<(std::ostream& os, CalcType val);

}  // namespace derived_time

// namespace time {

// OA_ENUM_MAPPERS_API
// std::ostream& operator<<(std::ostream& os, DayCountRule val);

// OA_ENUM_MAPPERS_API
// std::ostream& operator<<(std::ostream& os, AdjRule val);

// }  // namespace time

}  // namespace oa

#endif // !OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
