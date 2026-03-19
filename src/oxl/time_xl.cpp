#include "time_xl.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <format>
#include <stdexcept>
#include <string>

#include "derived_time/cashflow_gen/cashflow_gen.h"
#include "derived_time/cashflow_gen/cashflow_struct.h"
#include "derived_time/date_formula/business_date_formula.h"
#include "derived_time/date_formula/date_formula.h"
#include "derived_time/derived_time_enums.h"
#include "enum_mappers/time_enum_mappers.h"
#include "helpers/utils.h"
#include "static_data_cache/calendar_cache.h"
#include "time/calendar.h"
#include "time/date.h"
#include "time/day_count/day_counter_factory.h"
#include "time/time_enums.h"
#include "xl_api/excel_base.h"
#include "xl_api/xl_dictionary.h"
#include "xl_api/xloper_converter.h"
#include "xl_api/xl_utils.h"

namespace oxl {

	typedef oa::time::Date DateAlias;
	typedef xl_api::XLoperObj XLoperAlias;
	typedef oa::static_cache::CalendarCache CalCacheAlias;
	typedef oa::derived_time::CashflowGen CFGen;

	bool OxlIsBizDay(const xloper12* date, const xloper12* centers)
	{
		auto date_obj = xl_api::XloperToJulianInt(date);

		if (centers->xltype != xltypeStr)
		{
			throw std::invalid_argument(std::format("{}:{}:Invalid Centers was given please check the centers input",
				OA_SOURCE_LOCATION(), 
				__func__
				));
		}

		std::string centers_input = XLoperAlias::LPXloperToStr(centers);
		auto center_ptr = CalCacheAlias::RetrieveCache().GetCalendar(centers_input);

		return center_ptr->IsBusinessDay(date_obj);

	}

	bool OxlIsHolDay(const xloper12* date, const xloper12* centers)
	{
		auto date_obj = xl_api::XloperToJulianInt(date);

		if (centers->xltype != xltypeStr)
		{
			throw std::invalid_argument(std::format("{}:{}: Invalid Centers was given please check the centers input",
				OA_SOURCE_LOCATION(), 
				__func__
				));
		}

		std::string centers_input = XLoperAlias::LPXloperToStr(centers);
		auto center_ptr = CalCacheAlias::RetrieveCache().GetCalendar(centers_input);

		return center_ptr->IsHoliday(date_obj);

	}

	int OxlComputeDayCount(const xloper12* start_date, const xloper12* end_date, const xloper12* busines_day_count)
	{
		auto start_date_Obj = xl_api::ToDateObj(start_date);
		auto end_date_Obj = xl_api::ToDateObj(end_date);

		std::string biz_day_count = XLoperAlias::LPXloperToStr(busines_day_count);

		auto biz_day_count_enum = oa::enum_mappers::MapInputToDayCountEnum(biz_day_count);

		auto day_count_rule = oa::time::DayCounterFactory::GenerateDayCounter(biz_day_count_enum);

		return day_count_rule->
			DayCount(start_date_Obj, end_date_Obj);
	}

	double OxlComputeYearFraction(const xloper12* start_date, const xloper12* end_date, const xloper12* busines_day_count)
	{
		auto start_date_Obj = xl_api::ToDateObj(start_date);
		auto end_date_Obj = xl_api::ToDateObj(end_date);

		std::string biz_day_count = XLoperAlias::LPXloperToStr(busines_day_count);

		auto biz_day_count_enum = oa::enum_mappers::MapInputToDayCountEnum(biz_day_count);

		auto day_count_rule = oa::time::DayCounterFactory::GenerateDayCounter(biz_day_count_enum);

		return day_count_rule->
			YearFraction(start_date_Obj, end_date_Obj);
	}

	double OxlComputeDate(const xl_api::XlDictionary& dictionary)
	{
		DateAlias base_date = xl_api::ToDateObj(dictionary["Base_Date"]);
		auto tenor = std::get<std::string>(dictionary["Tenor"]);
		auto adj_rule = std::get<std::string>(dictionary["Adjustment_Rule"]);
		auto calendar = std::get<std::string>(dictionary["Calendar"]);
		auto date_formula = oa::derived_time::DateFormula(tenor, adj_rule, calendar);
		return xl_api::ToExcelDate(date_formula.Adjust(base_date));
	}

	double OxlAddBusinessDays(const xl_api::XlDictionary& dictionary)
	{
		DateAlias base_date = xl_api::ToDateObj(dictionary["Base_Date"]);
		auto num_of_days = static_cast<int>(std::get<double>(dictionary["Days"]));
		auto calendar = std::get<std::string>(dictionary["Calendar"]);
		auto date_formula = oa::derived_time::BusinessDateFormula(num_of_days, calendar);
		return xl_api::ToExcelDate(date_formula.Adjust(base_date));
	}

	xl_api::XlArray OxlGenerateCashflow(const xl_api::XlDictionary& dictionary)
	{
		if(xl_api::ValidCashflowGenDictionary(dictionary))
		{
			CFGen::Options opt{};
			auto start_date = xl_api::ToDateObj(dictionary["Start_Date"]);
			auto mat_date = xl_api::ToDateObj(dictionary["Mat_Date"]);
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

			return xl_api::ConvertCFStructToXlArray(cf_results, oa::derived_time::CashflowType::kFixed);
		}

		else
		{ 
			return {{"Invalid Dictionary. Required: Start_Date, Mat_Date, Notional, Rate, Day_Count_Frac, Date_Dir, Stub_Type, and Frequency!"}};
			
		}
	}

}
