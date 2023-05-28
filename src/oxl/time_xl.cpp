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

	bool OxlIsHolDay(LPXLOPER12 date, LPXLOPER12 centers)
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

		return center_ptr->IsHoliday(julian_date);

	}

	int OxlComputeDayCount(LPXLOPER12 start_date, LPXLOPER12 end_date, LPXLOPER12 busines_day_count)
	{
		int start_julian_date = DateAlias::kXlJulianOffSet;
		int end_julian_date = DateAlias::kXlJulianOffSet;

		//get julian
		if (start_date->xltype == xltypeNum)
		{
			start_julian_date += static_cast<int> (start_date->val.num);
		}

		else
		{
			throw std::invalid_argument(std::format("{}:{}",
				"Invlaid start date was given please check the date input", "excel_base_funcs.cpp line 91 ComputeDayCount()"));
		}

		//get end date julian		
		if (end_date->xltype == xltypeNum)
		{
			end_julian_date += static_cast<int> (end_date->val.num);
		}

		else
		{
			throw std::invalid_argument(std::format("{}:{}",
				"Invlaid end date was given please check the date input", "excel_base_funcs.cpp line 103 ComputeDayCount()"));
		}

		std::string biz_day_count = XLoperAlias::LPXloperToStr(busines_day_count);

		auto biz_day_count_enum = oa::enum_mappers::MapInputToDayCountEnum(biz_day_count);

		auto day_count_rule = oa::time::DayCounterFactory::GenerateDayCounter(biz_day_count_enum);

		return day_count_rule->
			DayCount(DateAlias::Date(start_julian_date), DateAlias::Date(end_julian_date));
	}

	double OxlComputeYearFraction(LPXLOPER12 start_date, LPXLOPER12 end_date, LPXLOPER12 busines_day_count)
	{
		int start_julian_date = DateAlias::kXlJulianOffSet;
		int end_julian_date = DateAlias::kXlJulianOffSet;

		//get julian
		if (start_date->xltype == xltypeNum)
		{
			start_julian_date += static_cast<int> (start_date->val.num);
		}

		else
		{
			throw std::invalid_argument(std::format("{}:{}",
				"Invlaid start date was given please check the date input", "excel_base_funcs.cpp line 91 ComputeDayCount()"));
		}

		//get end date julian		
		if (end_date->xltype == xltypeNum)
		{
			end_julian_date += static_cast<int> (end_date->val.num);
		}

		else
		{
			throw std::invalid_argument(std::format("{}:{}",
				"Invlaid end date was given please check the date input", "excel_base_funcs.cpp line 103 ComputeDayCount()"));
		}

		std::string biz_day_count = XLoperAlias::LPXloperToStr(busines_day_count);

		auto biz_day_count_enum = oa::enum_mappers::MapInputToDayCountEnum(biz_day_count);

		auto day_count_rule = oa::time::DayCounterFactory::GenerateDayCounter(biz_day_count_enum);

		return day_count_rule->
			YearFraction(DateAlias::Date(start_julian_date), DateAlias::Date(end_julian_date));
	}
}