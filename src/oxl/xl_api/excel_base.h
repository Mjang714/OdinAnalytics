#ifndef OXL_XL_API_EXCEL_BASE_H_
#define OXL_XL_API_EXCEL_BASE_H_

#include <memory>
#include <vector>

#include "derived_time/cashflow_gen/cashflow_struct.h"
#include "derived_time/date_formula/business_date_formula.h"
#include "derived_time/derived_time_enums.h"
#include "oxl/xl_api/xl_array.h"
#include "oxl/xl_api/xl_dictionary.h"

// forward decl to avoid bringing in XLCALL.H
struct xloper12;

namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const xloper12* data_range);
	xl_api::XlArray ConvertCFStructToXlArray(const std::vector<oa::derived_time::CashflowStruct>& cf_struct_array, oa::derived_time::CashflowType cf_type);
	std::shared_ptr<xl_api::XlDictionary> RetrieveXLDict(const xl_api::XlDictionary& dictionary, const std::string& chached_str);
	bool ValidBusinessDateDictionary(const xl_api::XlDictionary& dictionary);
	oa::derived_time::BusinessDateFormula GetBusinessDateFormulaFromDict(const xl_api::XlDictionary& dict, const std::string& date_rule_key);
}

#endif // !OXL_XL_API_EXCEL_BASE_H_
