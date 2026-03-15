#ifndef XL_UTILS_H_
#define XL_UTILS_H_

// note: must include Windows.h, XLCALL.H, FRAMEWRK.H due to templates
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>
#include <FRAMEWRK.H>  // include after XLCALL.H

#include <string>
#include "time/date.h"
#include "xloper_converter.h"
#include "oa/ctti.h"

// FIXME: register/unregister functions take fixed-size arrays and don't allow
// for more flexible specification of the registration inputs
namespace oxl::util
{
	class XlVarToDateVisitor
	{

	public:
		oa::time::Date operator()(const double& date_double) const
		{
			return oa::time::Date(static_cast<int>(date_double) + oa::time::Date::kXlJulianOffSet);
		}

		oa::time::Date operator()(const std::string& date_str) const
		{
			return oa::time::Date(date_str);
		}

		template <typename T>
		[[noreturn]] oa::time::Date operator()(const T& /* val */) const
		{
			throw std::invalid_argument(std::format("{}:{}:Invalid date was given please check the date input", 
				OA_SOURCE_LOCATION(),
				__func__
			));
		}
	};

}

namespace oxl::xl_api
{

	/// <summary>
	/// This is function that will unregister xll functions
	/// </summary>
	/// <param name="function_arr"></param>
	/// <param name="number_funcs"></param>
	template <size_t N>
	void UnregisterXLFunctions(const WCHAR* (&function_arr)[N][12])
	{
		for (size_t index = 0; index < N; index++)
		{
			Excel12f(xlfSetName, 0, 1, TempStr12(function_arr[index][2]));
		}
	}

	/// <summary>
	/// this is a function that is used to register excel functions
	/// </summary>
	/// <param name="DLL"></param>
	/// <param name="DLL"></param>
	/// <param name="function_arr"></param>
	/// <param name="number_funcs"></param>
	template <size_t N>
	void RegisterXLFunctions(XLOPER12& xDLL, const WCHAR* (&function_arr)[N][12])
	{
		for (size_t index = 0; index < N; index++)
		{
			Excel12f(xlfRegister,0,12,
				(LPXLOPER12) &xDLL,
				(LPXLOPER12)TempStr12(function_arr[index][0]),
				(LPXLOPER12)TempStr12(function_arr[index][1]),
				(LPXLOPER12)TempStr12(function_arr[index][2]),
				(LPXLOPER12)TempStr12(function_arr[index][3]),
				(LPXLOPER12)TempStr12(function_arr[index][4]),
				(LPXLOPER12)TempStr12(function_arr[index][5]),
				(LPXLOPER12)TempStr12(function_arr[index][6]),
				(LPXLOPER12)TempStr12(function_arr[index][7]),
				(LPXLOPER12)TempStr12(function_arr[index][8]),
				(LPXLOPER12)TempStr12(function_arr[index][9]),
				(LPXLOPER12)TempStr12(function_arr[index][10]),
				(LPXLOPER12)TempStr12(function_arr[index][11])
			);
		}
	}

	/// @brief a simple function to convert a date object to an excel date double date representation
	/// @param date 
	/// @return double representation of the date that excel can understand
	inline double ToExcelDate(const oa::time::Date& date)
	{
		return static_cast<double>(date.GetJulian() - oa::time::Date::kXlJulianOffSet);

	}
	
	/// @brief a simple function to convert an excel date double representation to a date object
	/// @param excel_date double representation of the date that excel can understand
	/// @return odin date object
	inline oa::time::Date ToDateObj(const xloper12* date)
	{
		switch(date->xltype)
		{
			case xltypeNum:
				return oa::time::Date(static_cast<int>( date->val.num) + oa::time::Date::kXlJulianOffSet);
			case xltypeStr:
				return oa::time::Date(XLoperObj::LPXloperToStr(date));
			default:
				throw std::invalid_argument(std::format("{}:{}:Invalid date was given please check the date input", 
					OA_SOURCE_LOCATION(),
					__func__
				));
		}
		
	}
	/// @brief This function takes in and excel date and converts it to julian integer date representation
	/// @param date 
	/// @return 
	inline int XloperToJulianInt(const xloper12* date) {
		int julian_date = oa::time::Date::kXlJulianOffSet;
		switch(date->xltype)
		{
			case xltypeNum:
				julian_date += static_cast<int>(date->val.num);
				break;
			case xltypeStr:
				julian_date += oa::time::Date(XLoperObj::LPXloperToStr(date)).GetJulian();
				break;
			default:
				throw std::invalid_argument(std::format("{}:{}:Invalid date was given please check the date input", 
					OA_SOURCE_LOCATION(),
					__func__
				));
		}
		return julian_date;
	}

	

	/// @brief converts an excel date double representation to a oding date object
	/// @param excel_date double representation of the date that excel can understand
	/// @return odin date object
	inline oa::time::Date ToDateObj(const XlVariant& date_var)
	{
		return std::visit(util::XlVarToDateVisitor{}, date_var);
	}
}
#endif // !XL_UTILS_H_
