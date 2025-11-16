#ifndef OXL_TIME_XL_H_
#define OXL_TIME_XL_H_

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdlib.h>

#include <format>
#include <iostream>
#include <memory>
#include <string>

#include "xlcall.h"
#include "framewrk.h"

#include <boost/algorithm/string.hpp>

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

namespace oxl
{
	bool OxlIsBizDay(LPXLOPER12 date, LPXLOPER12 centers);
	bool OxlIsHolDay(LPXLOPER12 date, LPXLOPER12 centers);
	int OxlComputeDayCount(LPXLOPER12 start_date, LPXLOPER12 end_date, LPXLOPER12 busines_day_count);
	double OxlComputeYearFraction(LPXLOPER12 start_date, LPXLOPER12 end_date, LPXLOPER12 busines_day_count);
	double OxlComputeDate(const xl_api::XlDictionary& dictionary);
}

#endif //OXL_TIME_XL_H_