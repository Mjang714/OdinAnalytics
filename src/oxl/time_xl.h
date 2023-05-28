#include <format>
#include <stdlib.h>
#define NOMINMAX
#include <memory>
#include <iostream>
#include <Windows.h>
#include <string>

#include "xlcall.h"
#include "framewrk.h"

#include "time/date.h"
#include "time/calendar.h"
#include "time/day_count/day_counter_factory.h"
#include "time/time_enums.h"

#include "static_data_cache/calendar_cache.h"
#include "enum_mappers/time_enum_mappers.h"


#include "xl_api/cache_xl_obj.h"
#include "xl_api/xl_variant.h"
#include "xl_api/xl_array.h"
#include "xl_api/xl_converter_funcs.h"

#include <boost/algorithm/string.hpp>

namespace oxl
{
	bool OxlIsBizDay(LPXLOPER12 date, LPXLOPER12 centers);
	bool OxlIsHolDay(LPXLOPER12 date, LPXLOPER12 centers);
	int OxlComputeDayCount(LPXLOPER12 start_date, LPXLOPER12 end_date, LPXLOPER12 busines_day_count);
	double OxlComputeYearFraction(LPXLOPER12 start_date, LPXLOPER12 end_date, LPXLOPER12 busines_day_count);
}
