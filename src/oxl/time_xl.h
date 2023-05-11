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

#include "static_data_cache/calendar_cache.h"

#include "xl_api/cache_xl_obj.h"
#include "xl_api/xl_variant.h"
#include "xl_api/xl_array.h"
#include "xl_api/xl_converter_funcs.h"

#include <boost/algorithm/string.hpp>

namespace oxl
{
	bool OxlIsBizDay(LPXLOPER12 date, LPXLOPER12 centers);
}
