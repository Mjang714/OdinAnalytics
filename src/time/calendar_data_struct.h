#ifndef ODINANALYTICS_TIME_CALENDAR_DATA_STRUCT_H_
#define ODINANALYTICS_TIME_CALENDAR_DATA_STRUCT_H_

#include<string>
#include <vector>

#include "date.h"
#include "time_enums.h"

namespace oa::ds
{
	
	// this struct is used as a data container for the constructor of the calendar constructor 
	// and follows google style sheet for structs hence no trailing _
	struct CalendarDataStruct
	{
		std::vector<oa::time::Date> list_of_holidays;
		std::vector<oa::time::Weekdays> weekends;
		std::string regions{ "NONE" };
		std::string currency {};
	};
}

#endif //ODINANALYTICS_TIME_CALENDAR_DATA_STRUCT_H_