#include "oxl/excel_base_funcs.h"

namespace oxl
{
	xl_api::CachedObjVar OxlCreateArray(const LPXLOPER12& xloper_input)
	{
		xl_api::XlArray xl_array = xl_api::XLoperObj::LPXloperToXlArray(xloper_input);
		//preferred to use make_shared 
		return std::make_shared<xl_api::XlArray>(xl_array);
	}

	xl_api::CachedObjVar OxlCreateDict(const LPXLOPER12& xloper_input)
	{
		xl_api::XlDictionary xl_dictionary = XlOper::LPXloperToDictionary(xloper_input);
		//preferred to use make_shared 
		return std::make_shared<xl_api::XlDictionary>(xl_dictionary);
	}

	xl_api::CachedObjVar OxlCreateDict(const LPXLOPER12& xloper_key, const LPXLOPER12& xloper_values)
	{
		xl_api::CachedObjVar cached_obj;
		if (XlOper::IsMulti(xloper_key) && XlOper::IsMulti(xloper_values))
		{	
			xl_api::XlDictionary xl_dictionary = XlOper::LPXloperToDictionary(xloper_key, xloper_values);
			cached_obj = std::make_shared<xl_api::XlDictionary>(xl_dictionary);
		}

		else
		{
			cached_obj = OxlCreateDict(xloper_key);
		}

		return cached_obj;
	}

	xl_api::XlArray OxlDisplay(const LPXLOPER12& xloper_input)
	{
		std::string handle_str = oxl::xl_api::XLoperObj::LPXloperToStr(xloper_input);

		std::string key = oxl::xl_api::XlCacheObj::GetKeyFromHandle(handle_str);

		if (!xl_api::XlCacheObj::IsHandle(key))
		{
			throw std::runtime_error(std::format("{}{}:{}", handle_str, " is not a valid cache handle please check input!", "excel_base_funcs.cpp line 19 OxlDisplay()"));
		}
		auto cache_variant = oxl::xl_api::XlCacheObj::GetVariant(key);

		std::shared_ptr<xl_api::XlArray> xl_array;
		if (std::holds_alternative<std::shared_ptr<xl_api::XlDictionary>>(cache_variant))
		{
			auto& xl_dictionary = std::get<std::shared_ptr<xl_api::XlDictionary>>(cache_variant);
			xl_array = std::make_shared<xl_api::XlArray> (XlDictionaryToXlArray(*xl_dictionary));
		}

		else
		{
			xl_array = std::get<std::shared_ptr<xl_api::XlArray>>(cache_variant);
		}

		return *xl_array;
	} 
}