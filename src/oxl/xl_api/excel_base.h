#ifndef OXL_XL_API_EXCEL_BASE_H_
#define OXL_XL_API_EXCEL_BASE_H_

#include <stdlib.h>
#define NOMINMAX
#include <Windows.h>
#include <string>
#include <memory>

#include "xlcall.h"
#include "framewrk.h"

#include "xloper_converter.h"
#include "xl_array.h"
#include "cache_xl_obj.h"

#include "derived_time/cashflow_gen/cashflow_struct.h"
#include "derived_time/derived_time_enums.h"
#include "derived_time/date_formula/business_date_formula.h"

namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const LPXLOPER12& data_range);
	xl_api::XlArray ConvertCFStructToXlArray(const std::vector<oa::derived_time::CashflowStruct>& cf_struct_array, oa::derived_time::CashflowType cf_type);
	std::shared_ptr<xl_api::XlDictionary> RetrieveXLDict(const xl_api::XlDictionary& dictionary, const std::string& chached_str);
	bool ValidBusinessDateDictionary(const xl_api::XlDictionary& dictionary);
	oa::derived_time::BusinessDateFormula GetBusinessDateFormulaFromDict(const xl_api::XlDictionary& dict, const std::string& date_rule_key);
}

#endif // !OXL_XL_API_EXCEL_BASE_H_
