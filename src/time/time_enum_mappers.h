#ifndef ODINANALYTICS_TIME_TIME_ENUM_MAPPERS_H_
#define ODINANALYTICS_TIME_TIME_ENUM_MAPPERS_H_

#include "oa/dllexport.h"
#include "time/time_enums.h"

#include <string>
#include <iostream>

namespace oa::time {


    OA_TIME_API
    oa::time::DayCountRule MapInputToDayCountEnum(const std::string& input_str);

    OA_TIME_API
    oa::time::AdjRule MapInputToDayAdjustEnum(const std::string& input_str);

    OA_TIME_API
    std::ostream& operator<<(std::ostream& os, DayCountRule val);

    OA_TIME_API
    std::ostream& operator<<(std::ostream& os, AdjRule val);

}  // namespace time


#endif  // ODINANALYTICS_TIME_TIME_ENUM_MAPPERS_H_  
