#include <format>
#include <stdlib.h>
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include <string>

#include "xlcall.h"
#include "framewrk.h"

#include "xl_variant.h"  
#include "xl_array.h"
#include "xl_dictionary.h"
#include "xl_converter_funcs.h"




#ifndef OXL_XL_API_XLOPER_CONVERTER_H_
#define OXL_XL_API_XLOPER_CONVERTER_H_



namespace oxl::xl_api
{
	using XloperVar = std::variant<double, std::string, bool, LPXLOPER12>;

	//this class is responsible for converting data types to LPXLOPER12 only! 
	class XLoperObj
	{
		public:
			//define a null constructor for this class
			XLoperObj() = default;

			int ToInteger();
			double ToDouble();

			//constructors to store the value for XLoper type
			explicit XLoperObj(const double input);
			explicit XLoperObj(const char* input);
			explicit XLoperObj(const LPXLOPER12 input);

			/// <summary>
			/// a function to check if a given LPXLOPER12 is missing data that we can extract
			/// </summary>
			/// <param name="input"></param>
			/// <returns></returns>
			static bool IsEmpty(const LPXLOPER12 input);

			/// <summary>
			/// check to see if the LPXLOPER is multi from excel
			/// </summary>
			/// <param name="input"></param>
			/// <returns></returns>
			static bool IsMulti(const LPXLOPER12 input);
			
			/// <summary>
			/// checks to see if the data is empty.
			/// </summary>
			/// <returns></returns>
			bool HasData(void);

			/// <summary>
			/// A class of static functions that will take a given LPXOPER12 that is already allocated as input and a value
			/// and populates the value 
			/// </summary>
			/// <param name="value"></param>
			/// <returns></returns>
			static void ConvertToLPXloper(double value, LPXLOPER12 xl_oper_res);
			static void ConvertToLPXloper(bool value, LPXLOPER12 xl_oper_res);
			static void ConvertToLPXloper(const std::string& value, LPXLOPER12 xl_oper_res);
			static void ConvertToLPXloper(const XlArray& value, LPXLOPER12 xl_oper_res);

			/// <summary>
			/// takes in LPXLOPER12 and converts it XlArray 
			/// </summary>
			/// <param name="xl_oper"></param>
			/// <returns></returns>
			static XlArray LPXloperToXlArray(const LPXLOPER12& xl_oper);

			static XlDictionary LPXloperToDictionary(const LPXLOPER12& xl_oper);
			static XlDictionary LPXloperToDictionary(const LPXLOPER12& xl_oper_keys, const LPXLOPER12& xl_oper_values);

			static std::string LPXloperToStr(const LPXLOPER12& xl_oper);

			//static std::string GenHandleName(const CachedObjVar& cached_obj);
			static std::string CellName(void);

			/// <summary>
			/// Takes in a utf8 string (wtd::string) and converts it to wide string 
			/// resource: https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar
			/// </summary>
			/// <param name="value_str"></param>
			/// <returns></returns>
			static std::wstring ToWString(const std::string& value_str);

			/// <summary>
			/// takes a wide string and converts it utf8 string
			/// resource: https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar
			/// </summary>
			/// <param name="value_str"></param>
			/// <returns></returns>
			static std::string ToUTF8String(const std::wstring& value_str);

		private:
			XloperVar m_data_;

	};
}

#endif // !OXL_XL_API_XLOPER_CONVERTER_H_





