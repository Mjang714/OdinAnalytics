#include <format>
#include <stdlib.h>
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include <string>

#include "xlcall.h"
#include "framewrk.h"

#include "xl_api/cache_xl_obj.h"
#include "xl_api/xl_variant.h"
#include "xl_api/xl_array.h"
#include "xl_api/xl_converter_funcs.h"


#ifndef OXL_EXCEL_BASE_FUNCTIONS_H
#define OXL_EXCEL_BASE_FUNCTIONS_H

namespace oxl
{
	typedef xl_api::XLoperObj XlOper;
	typedef xl_api::XlArray XlArray;

	/// <summary>
	/// create a cahedobjvar that stores the conveted type from the xloper obj
	/// </summary>
	/// <param name="xloper_input"></param>
	/// <returns></returns>
	xl_api::CachedObjVar OxlCreateArray(const LPXLOPER12 &xloper_input);
	
	/// <summary>
	/// takes in multi data type and turns it into ta dictionary
	/// </summary>
	/// <param name="xloper_input"></param>
	/// <returns></returns>
	xl_api::CachedObjVar OxlCreateDict(const LPXLOPER12& xloper_input);

	/// <summary>
	/// takes in two 1XN or Nx1 matrix or one large Nx2 and 2xN  
	/// </summary>
	/// <param name="xloper_key"></param>
	/// <param name="xloper_values"></param>
	/// <returns></returns>
	xl_api::CachedObjVar OxlCreateDict(const LPXLOPER12& xloper_key, const LPXLOPER12& xloper_values);

	/// <summary>
	/// takes in a handle string and spits outthe cached results as an LPXLOPER12 
	/// </summary>
	/// <param name="xloper_input"></param>
	/// <returns></returns>
	xl_api::XlArray OxlDisplay(const LPXLOPER12 &xloper_input);
}

#endif // !OXL_EXCEL_BASE_FUNCTIONS_H

