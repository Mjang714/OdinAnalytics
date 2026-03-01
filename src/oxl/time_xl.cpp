#include "time_xl.h"
#include "derived_time/date_formula/date_formula.h"
#include "derived_time/date_formula/business_date_formula.h"
#include "derived_time/derived_time_enums.h"
#include "derived_time/cashflow_gen/cashflow_struct.h"
#include "derived_time/derived_time_enums.h"
#include "derived_time/cashflow_gen/cashflow_gen.h"
#include "helpers/utils.h"

namespace oxl {
	typedef oa::time::Date DateAlias;
	typedef oxl::xl_api::XLoperObj XLoperAlias;
	typedef oa::static_cache::CalendarCache CalCacheAlias;
	typedef oa::derived_time::CashflowGen CFGen;

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

	double OxlComputeDate(const xl_api::XlDictionary& dictionary)
	{
		auto j_date = static_cast<int> (std::get<double>(dictionary["Base_Date"])) + DateAlias::kXlJulianOffSet;
		DateAlias base_date(j_date);
		auto tenor = std::get<std::string>(dictionary["Tenor"]);
		auto adj_rule = std::get<std::string>(dictionary["Adjustment_Rule"]);
		auto calendar = std::get<std::string>(dictionary["Calendar"]);
		auto date_formula = oa::derived_time::DateFormula(tenor, adj_rule, calendar);
		return static_cast<double>(date_formula.Adjust(base_date).GetJulian() - DateAlias::kXlJulianOffSet);
	}

	double OxlAddBusinessDays(const xl_api::XlDictionary& dictionary)
	{
		auto j_date = static_cast<int> (std::get<double>(dictionary["Base_Date"])) + DateAlias::kXlJulianOffSet;
		DateAlias base_date(j_date);
		auto num_of_days = static_cast<int>(std::get<double>(dictionary["Days"]));
		auto calendar = std::get<std::string>(dictionary["Calendar"]);
		auto date_formula = oa::derived_time::BusinessDateFormula(num_of_days, calendar);
		return static_cast<double>(date_formula.Adjust(base_date).GetJulian() - DateAlias::kXlJulianOffSet);
	}

	xl_api::XlArray OxlGenerateCashflow(const xl_api::XlDictionary& dictionary)
	{
		CFGen::Options opt{};
		auto start_date = DateAlias(static_cast<int> (std::get<double>(dictionary["Start_Date"])) + DateAlias::kXlJulianOffSet);
		auto mat_date = DateAlias(static_cast<int> (std::get<double>(dictionary["Mat_Date"])) + DateAlias::kXlJulianOffSet);
		auto notional = std::get<double>(dictionary["Notional"]);
		auto rate = std::get<double>(dictionary["Rate"]);
		auto day_cnt_frac = oa::enum_mappers::MapInputToDayCountEnum(std::get<std::string>(dictionary["Day_Count_Frac"]));
		auto freq_tenor_str = std::get<std::string>(dictionary["Frequency"]);
		auto freq = (oa::utils::CheckTenorStr(freq_tenor_str)) ? oa::time::Tenor(freq_tenor_str) : CFGen::MapResetFreqEnumToTenor(oa::enum_mappers::MapInputToFreq(freq_tenor_str));
		opt.date_direction(oa::enum_mappers::MapInputToDateDir(std::get<std::string>(dictionary["Date_Dir"])));
		opt.stub_type(oa::enum_mappers::MapInputToStub(std::get<std::string>(dictionary["Stub_Type"])));
		
		if(dictionary.Contains("Fixing_Date_Rule"))
		{
			opt.fix_adjustment(GetBusinessDateFormulaFromDict(dictionary, "Fixing_Date_Rule"));
		}

		if(dictionary.Contains("Payment_Date_Rule"))
		{
			opt.pay_adjustment(GetBusinessDateFormulaFromDict(dictionary, "Payment_Date_Rule"));
		}

		//case for when we want the same adjusment rule for both the start and end date
		if(dictionary.Contains("Acc_Adj_Rule"))
		{
			opt.start_adjustment(GetBusinessDateFormulaFromDict(dictionary, "Acc_Adj_Rule"))
			.end_adjustment(GetBusinessDateFormulaFromDict(dictionary, "Acc_Adj_Rule"));
		}

		else if(dictionary.Contains("Start_Adj_Rule") || dictionary.Contains("End_Adj_Rule"))
		{
			if(dictionary.Contains("End_Adj_Rule"))
			{
				opt.end_adjustment(GetBusinessDateFormulaFromDict(dictionary, "End_Adj_Rule"));
			}

			if(dictionary.Contains("Start_Adj_Rule"))
			{
				opt.start_adjustment(GetBusinessDateFormulaFromDict(dictionary, "Start_Adj_Rule"));
			}
		}
		

		auto cf_results = CFGen::CreateFixedCashflows(start_date, mat_date,freq, notional, rate, day_cnt_frac, opt);

		return oxl::xl_api::ConvertCFStructToXlArray(cf_results, oa::derived_time::CashflowType::kFixed);
	}

}