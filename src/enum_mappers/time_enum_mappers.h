#include <string>
#include <ostream>

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

}  // namespace oa::enum_mappers


namespace oa::derived_time {

    OA_ENUM_MAPPERS_API
    std::ostream& operator<<(std::ostream& os, DateDirection val);

    OA_ENUM_MAPPERS_API
    std::ostream& operator<<(std::ostream& os, Frequency val);

    OA_ENUM_MAPPERS_API
    std::ostream& operator<<(std::ostream& os, StubType val);

    OA_ENUM_MAPPERS_API
    std::ostream& operator<<(std::ostream& os, CalcType val);
}

//not sure if this makes sense to keep there was thinking of moving it time_enum.h bu then we would ened to add a cpp file 
namespace oa::time {

OA_ENUM_MAPPERS_API
std::ostream& operator <<(std::ostream& os, DayCountRule val);

OA_ENUM_MAPPERS_API
std::ostream& operator <<(std::ostream& os, AdjRule val);

}  // namespace oa::time
#endif // !OA_ENUM_MAPPERS_TIME_ENUM_MAPPERS_H_
