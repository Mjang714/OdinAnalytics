#ifndef XL_UTILS_H_
#define XL_UTILS_H_

#include "xl_uitls.h"

#include <stdlib.h>
#define NOMINMAX
#include <Windows.h>
#include <string>

#include "xlcall.h"
#include "framewrk.h"


namespace oxl::xl_api
{
	/// <summary>
	/// This is function that will unregister xll functions
	/// Note: it is important for this work correctly that you set
	/// Conformance Mode to No (/permissive)
	/// </summary>
	/// <param name="function_arr"></param>
	/// <param name="number_funcs"></param>
	void UnregisterXLFunctions(const WCHAR* function_arr[][12], size_t number_funcs)
	{
		for (size_t index = 0; index <number_funcs; index++)
		{
			Excel12f(xlfSetName, 0, 1, TempStr12(function_arr[index][2]));
		}
	}

	/// <summary>
	/// this is a function that is used to register excel functions
	/// Note: it is important for this work correctly that you set
	/// Conformance Mode to No (/permissive)
	/// </summary>
	/// <param name="DLL"></param>
	/// <param name="DLL"></param>
	/// <param name="function_arr"></param>
	/// <param name="number_funcs"></param>
	void RegisterXLFunctions(XLOPER12& xDLL, const WCHAR* function_arr[][12], size_t number_funcs)
	{
		for (size_t index = 0; index < number_funcs; index++)
		{
			Excel12f(xlfRegister,	0,12,
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
