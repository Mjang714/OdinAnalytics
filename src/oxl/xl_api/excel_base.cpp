#include "excel_base.h"

#include "helpers/utils.h"
#include "oxl/xl_api/cache_xl_obj.h"
#include "oxl/xl_api/xl_array.h"
#include "oxl/xl_api/xloper_converter.h"

namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const xloper12* data_range)
	{
		XlArray xl_array = XLoperObj::LPXloperToXlArray(data_range);
		return std::shared_ptr<XlArray>(new XlArray(xl_array));
	}

	xl_api::XlArray ConvertCFStructToXlArray(const std::vector<oa::derived_time::CashflowStruct>& cf_struct_array, oa::derived_time::CashflowType cf_type)
	{

		switch(cf_type)
		{

			case oa::derived_time::CashflowType::kFixed:
			{
				xl_api::XlArray xl_results(cf_struct_array.size() + 1, 14);
				xl_results(0, 0) = std::string("Unadj_Start_Date");
				xl_results(0, 1) = std::string("Unadj_End_Date");
				xl_results(0, 2) = std::string("Start_Date");
				xl_results(0, 3) = std::string("End_Date");
				xl_results(0, 4) = std::string("Fixing_Date");
				xl_results(0, 5) = std::string("Payment_Date");
				xl_results(0, 6) = std::string("Notional");
				xl_results(0, 7) = std::string("Rate");
				xl_results(0, 8) = std::string("Fwd_Cashflow_PV");
				xl_results(0, 9) = std::string("Cashflow_NPV");
				xl_results(0, 10) = std::string("Day_Count");
				xl_results(0, 11) = std::string("Day_Count_Fraction");
				xl_results(0, 12) = std::string("Currency");
				xl_results(0, 13) = std::string("Cashflow_Type");
				//may want to put this into a seperate function that converts different cashflows into output idk feel free to comment.
				for(size_t i = 1; i < xl_results.rows(); i++) {
					auto cf_index = i - 1;
					xl_results(i, 0) = static_cast<double>(cf_struct_array.at(cf_index).unadj_start_date.ToExcelJulian());
					xl_results(i, 1) = static_cast<double>(cf_struct_array.at(cf_index).unadj_end_date.ToExcelJulian());
					xl_results(i, 2) = static_cast<double>(cf_struct_array.at(cf_index).start_date.ToExcelJulian());
					xl_results(i, 3) = static_cast<double>(cf_struct_array.at(cf_index).end_date.ToExcelJulian());
					xl_results(i, 4) = static_cast<double>(cf_struct_array.at(cf_index).fixing_date.ToExcelJulian());
					xl_results(i, 5) = static_cast<double>(cf_struct_array.at(cf_index).payment_date.ToExcelJulian());
					xl_results(i, 6) = cf_struct_array.at(cf_index).notional;
					xl_results(i, 7) = cf_struct_array.at(cf_index).rate;
					xl_results(i, 8) = cf_struct_array.at(cf_index).cashflow_amount;
					xl_results(i, 9) = cf_struct_array.at(cf_index).npv_cashflow_amount;
					xl_results(i, 10) = static_cast<double>(cf_struct_array.at(cf_index).days);
					xl_results(i, 11) = cf_struct_array.at(cf_index).day_count_fraction;
					xl_results(i, 12) = oa::utils::GetCleanName<oa::derived_time::Currency>(cf_struct_array.at(cf_index).cf_curr);
					xl_results(i, 13) = oa::utils::GetCleanName<oa::derived_time::CashflowType>(cf_struct_array.at(cf_index).cf_type);
				}
				return xl_results;
			}

			default:
			{
				xl_api::XlArray xl_results(1,1);
				xl_results(1,1) = std::string("Invlaid input was given");
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
		auto cache_variant = oxl::xl_api::XlCacheObj::GetVariant(fixing_dict_str_key);
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
