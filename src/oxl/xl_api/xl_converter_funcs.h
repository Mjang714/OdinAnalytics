#ifndef OXL_XL_API_XL_CONVERTER_FUNCS_H_
#define OXL_XL_API_XL_CONVERTER_FUNCS_H_

#include "xl_array.h"
#include "xl_dictionary.h"

namespace oxl::xl_api
{
	/// <summary>
	/// takes in XlArray class and converts it to a XlDictionary
	/// </summary>
	/// <param name="xl_array"></param>
	/// <returns></returns>
	XlDictionary XlArrayToXlDictionary(const XlArray& xl_array);

	/// <summary>
	/// converts a dictionary to array 
	/// </summary>
	/// <param name="xl_dictionary"></param>
	/// <returns></returns>
	XlArray XlDictionaryToXlArray(const XlDictionary& xl_dictionary);
}
#endif // !OXL_XL_API_XL_CONVERTER_FUNCS_H_



