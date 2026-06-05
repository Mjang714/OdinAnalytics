#ifndef ODINANALYTICS_TIME_TIME_ENUM_MAPPERS_H_
#define ODINANALYTICS_TIME_TIME_ENUM_MAPPERS_H_

#include <iostream>
#include <string>

#include "oa/dllexport.h"
#include "time/time_enums.h"

namespace oa::time {

OA_TIME_API
oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str);

OA_TIME_API
oa::time::AdjRule MapInputToDayAdjustEnum(const std::string& input_str);

}  // namespace oa::time

#endif  // ODINANALYTICS_TIME_TIME_ENUM_MAPPERS_H_  
