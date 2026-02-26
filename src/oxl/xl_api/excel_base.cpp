#include "excel_base.h"

#include "helpers/utils.h"


namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const LPXLOPER12& data_range)
	{
		XlArray xl_array = XLoperObj::LPXloperToXlArray(data_range);
		return std::shared_ptr<XlArray>(new XlArray(xl_array));
	}

	xl_api::XlArray ConvertCFStructToXlArray(const std::vector<oa::derived_time::CashflowStruct>& cf_struct_array)
	{
		xl_api::XlArray xl_results(cf_struct_array.size() + 1, 14);
		//may want to put this into a seperate function that converts different cashflows into output idk feel free to comment. 
		for(size_t i = 0; i < cf_struct_array.size(); i++) {
			xl_results(i, 0) = static_cast<double>(cf_struct_array.at(i).unadj_start_date.ToExcelJulian());
			xl_results(i, 1) = static_cast<double>(cf_struct_array.at(i).unadj_end_date.ToExcelJulian());
			xl_results(i, 2) = static_cast<double>(cf_struct_array.at(i).start_date.ToExcelJulian());
			xl_results(i, 3) = static_cast<double>(cf_struct_array.at(i).end_date.ToExcelJulian());
			xl_results(i, 4) = static_cast<double>(cf_struct_array.at(i).fixing_date.ToExcelJulian());
			xl_results(i, 5) = static_cast<double>(cf_struct_array.at(i).payment_date.ToExcelJulian());
			xl_results(i, 6) = cf_struct_array.at(i).notional;
			xl_results(i, 7) = cf_struct_array.at(i).rate;
			xl_results(i, 8) = cf_struct_array.at(i).cashflow_amount;
			xl_results(i, 9) = cf_struct_array.at(i).npv_cashflow_amount;
			xl_results(i, 10) = static_cast<double>(cf_struct_array.at(i).days);
			xl_results(i, 11) = cf_struct_array.at(i).day_count_fraction;
			xl_results(i, 12) = oa::utils::GetCleanName<oa::derived_time::Currency>(cf_struct_array.at(i).cf_curr);
			xl_results(i, 13) = oa::utils::GetCleanName<oa::derived_time::CashflowType>(cf_struct_array.at(i).cf_type);

		}
		return xl_results;
	}
}