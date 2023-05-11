#include "time_xl.h"

namespace oxl
{
	typedef oa::time::Date DateAlias;
	typedef oxl::xl_api::XLoperObj XLoperAlias;
	typedef oa::static_cache::CalendarCache CalCacheAlias;

	bool OxlIsBizDay(LPXLOPER12 date, LPXLOPER12 centers)
	{
		int julian_date = DateAlias::kXlJulianOffSet;	
		if (date->xltype == xltypeStr)
		{
			std::string date_str = XLoperAlias::LPXloperToStr(date);;
			julian_date += DateAlias::Date(date_str).GetJulian();
		}

		else if (date->xltype == xltypeNum)
		{
			julian_date += static_cast<int> (date->val.num);
		}

		else
		{
			throw std::invalid_argument(std::format("{}:{}", 
				"Invlaid date was given please check the date input", "excel_base_funcs.cpp line 23 OxlIsBizDay()"));
		}
		

		if (centers->xltype != xltypeStr)
		{
			throw std::invalid_argument(std::format("{}:{}",
				"Invlaid Centers was given please check the centers input", "excel_base_funcs.cpp line 37 OxlIsBizDay()"));
		}

		std::string centers_input = XLoperAlias::LPXloperToStr(centers);
		auto center_ptr = CalCacheAlias::RetrieveCache().GetCalendar(centers_input);

		return center_ptr->IsBusinessDay(julian_date);
		
	}
}