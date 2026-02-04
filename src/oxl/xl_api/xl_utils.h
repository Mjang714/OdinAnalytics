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

// FIXME: register/unregister functions take fixed-size arrays and don't allow
// for more flexible specification of the registration inputs

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
	inline void RegisterXLFunctions(XLOPER12& xDLL, const WCHAR* (&function_arr)[N][12])
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
}
#endif // !XL_UTILS_H_
