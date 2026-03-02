#ifndef OXL_EXCEL_BASE_FUNCTIONS_H
#define OXL_EXCEL_BASE_FUNCTIONS_H

#include "xl_api/cache_xl_obj.h"

// forward decl to avoid bringing in XLCALL.H
struct xloper12;

namespace oxl
{
	/// <summary>
	/// create a cahedobjvar that stores the conveted type from the xloper obj
	/// </summary>
	/// <param name="xloper_input"></param>
	/// <returns></returns>
	xl_api::CachedObjVar OxlCreateArray(const xloper12* xloper_input);

	/// <summary>
	/// takes in multi data type and turns it into ta dictionary
	/// </summary>
	/// <param name="xloper_input"></param>
	/// <returns></returns>
	xl_api::CachedObjVar OxlCreateDict(const xloper12* xloper_input);

	/// <summary>
	/// takes in two 1XN or Nx1 matrix or one large Nx2 and 2xN
	/// </summary>
	/// <param name="xloper_key"></param>
	/// <param name="xloper_values"></param>
	/// <returns></returns>
	xl_api::CachedObjVar OxlCreateDict(const xloper12* xloper_key, const xloper12* xloper_values);

	/// <summary>
	/// takes in a handle string and spits outthe cached results as an LPXLOPER12
	/// </summary>
	/// <param name="xloper_input"></param>
	/// <returns></returns>
	xl_api::XlArray OxlDisplay(const xloper12* xloper_input);
}

#endif // !OXL_EXCEL_BASE_FUNCTIONS_H
