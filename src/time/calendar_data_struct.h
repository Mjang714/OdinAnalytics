#ifndef OPENANALYTICS_TIME_CALENDAR_DATA_STRUCT_H
#define OPENANALYTICS_TIME_CALENDAR_DATA_STRUCT_H

#include<string>
#include <vector>

#include "date.h"
#include "time_enums.h"

namespace oa
{
	// ds = data struct
	namespace ds
	{
		// this struct is used as a data container for the constructor of the calendar constructor 
		// and follows google style sheet for structs hence no trailing _
		struct CalendarDataStruct
		{
			std::vector<time::Date> list_of_holidays;
			std::vector<time::Weekdays> weekends;
			std::string regions{ "NONE" };
		};
	}
}

#endif //OPENANALYTICS_TIME_CALENDAR_DATA_STRUCT_H