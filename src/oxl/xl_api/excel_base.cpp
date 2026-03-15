#include "excel_base.h"

#include "helpers/utils.h"
#include "oxl/xl_api/cache_xl_obj.h"
#include "oxl/xl_api/xl_array.h"
#include "oxl/xl_api/xloper_converter.h"
#include "oxl/xl_api/xl_utils.h"
#include "time/date.h"

namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const xloper12* data_range)
	{
		return std::make_shared<XlArray>(XLoperObj::LPXloperToXlArray(data_range));
	}

	xl_api::XlArray ConvertCFStructToXlArray(const std::vector<oa::derived_time::CashflowStruct>& cf_struct_array, oa::derived_time::CashflowType cf_type)
	{

		switch(cf_type)
		{

			case oa::derived_time::CashflowType::kFixed:
			{
				xl_api::XlArray xl_results(cf_struct_array.size() + 1, 14);
				xl_results[0] = {"Unadj_Start_Date", "Unadj_End_Date", "Start_Date", "End_Date", "Fixing_Date", "Payment_Date", "Notional", "Rate", "Fwd_Cashflow_PV", "Cashflow_NPV", "Day_Count", "Day_Count_Fraction", "Currency", "Cashflow_Type"};
				//may want to put this into a seperate function that converts different cashflows into output idk feel free to comment.
				for(size_t i = 1; i < xl_results.rows(); i++) {
					const auto& cf = cf_struct_array[i - 1u];
					xl_results[i] = {
						ToExcelDate(cf.unadj_start_date),
						ToExcelDate(cf.unadj_end_date),
						ToExcelDate(cf.start_date),
						ToExcelDate(cf.end_date),
						ToExcelDate(cf.fixing_date),
						ToExcelDate(cf.payment_date),
						cf.notional,
						cf.rate,
						cf.cashflow_amount,
						cf.npv_cashflow_amount,
						static_cast<double>(cf.days),
						cf.day_count_fraction,
						oa::utils::GetCleanName<oa::derived_time::Currency>(cf.cf_curr),
						oa::utils::GetCleanName<oa::derived_time::CashflowType>(cf.cf_type)
					};
				}
				return xl_results;
			}

			default:
			{
				xl_api::XlArray xl_results(1,1);
				xl_results(0,0) = std::string("Invalid input was given");
				return xl_results;
			}


		}
	}

	std::shared_ptr<xl_api::XlDictionary> RetrieveXLDict(const xl_api::XlDictionary& dictionary, const std::string& chached_str)
	{

		auto fixing_dict_handle = std::get<std::string>(dictionary[chached_str]);
		auto fixing_dict_str_key = oxl::xl_api::XlCacheObj::GetKeyFromHandle(fixing_dict_handle);
		if (!xl_api::XlCacheObj::IsDictionary(fixing_dict_str_key))
		{
			throw std::runtime_error(std::format("{}{}", fixing_dict_handle, " is not a valid cached dictionary handle please check input!"));
		}
		auto cache_variant = XlCacheObj::GetVariant(fixing_dict_str_key);
		auto xl_dictionary = std::get<std::shared_ptr<xl_api::XlDictionary>>(cache_variant);
		return xl_dictionary;


	}

	bool ValidBusinessDateDictionary(const xl_api::XlDictionary& dict)
	{
		return dict.Contains("Days") && dict.Contains("Calendar");
	}

	oa::derived_time::BusinessDateFormula GetBusinessDateFormulaFromDict(const xl_api::XlDictionary& dict, const std::string& date_rule_key)
	{
		auto date_rule_dict = RetrieveXLDict(dict, date_rule_key);
		if (!ValidBusinessDateDictionary(*date_rule_dict))
		{
			throw std::runtime_error(std::format("{}{}{}", "The dictionary stored in ", date_rule_key, " is not a valid business date dictionary please check input!"));
		}
		auto num_of_days = static_cast<int>(std::get<double>((*date_rule_dict)["Days"]));
		auto calendar = std::get<std::string>((*date_rule_dict)["Calendar"]);
		return oa::derived_time::BusinessDateFormula(num_of_days, calendar);
	}

	bool ValidCashflowGenDictionary(const xl_api::XlDictionary& dictionary) {
		return dictionary.Contains("Start_Date") && dictionary.Contains("Mat_Date") && dictionary.Contains("Notional") && dictionary.Contains("Rate") && dictionary.Contains("Day_Count_Frac") && dictionary.Contains("Frequency") && dictionary.Contains("Date_Dir") && dictionary.Contains("Stub_Type");
	}
}
