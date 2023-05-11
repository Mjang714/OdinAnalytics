#include "oxl/xl_api/xl_uitls.h"
#include "oxl/auto_gen/autogen_excel_reg_funcs.h"
#include "oxl/auto_gen/auto_gen_time_reg_fns.h"



namespace oxl
{

	//	Registers the functions when the xll is laoded 
	extern "C" __declspec(dllexport)
	int WINAPI xlAutoOpen(void)
	{
		XLOPER12 xDLL;

		Excel12f(xlGetName, &xDLL, 0);

		oxl::xl_api::RegisterXLFunctions(xDLL, excel_base_ngFuncs, excel_base_num_reg_functions_rows);
		oxl::xl_api::RegisterXLFunctions(xDLL, time_ngFuncs, time_num_reg_functions_rows);

		/* Free the XLL filename */
		Excel12f(xlFree, 0, 1,   (LPXLOPER12)&xDLL);

		return 1;
	}

	// Unregisters the functions whent eh xll is unloaded
	extern "C" __declspec(dllexport)
	int WINAPI xlAutoClose(void)
	{
		oxl::xl_api::UnregisterXLFunctions(excel_base_ngFuncs, excel_base_num_reg_functions_rows);
		return 1;
	}

	extern "C" __declspec(dllexport)
	void WINAPI xlAutoFree12(LPXLOPER12 xl_oper)
	{
		//free a basic string freeing a XLOPER12 does free XCHAR* so you have free XCHAR
		if (xl_oper->xltype == (xlbitDLLFree | xltypeStr))
		{
			free(xl_oper->val.str);
		}

		else if (xl_oper->xltype == (xlbitDLLFree | xltypeRef))
		{
			free(xl_oper->val.mref.lpmref);
		}

		//free any xchar* that were allocated by the dll XLOPER12 does free XCHAR* so you have free XCHAR
		else if (xl_oper -> xltype == (xltypeMulti | xlbitDLLFree))
		{
			size_t array_size = xl_oper->val.array.rows * xl_oper->val.array.columns;
			for (size_t i = 0; i < array_size; i++)
			{
				if (xl_oper->val.array.lparray[i].xltype == (xlbitDLLFree | xltypeStr))
				{
					free(xl_oper->val.array.lparray[i].val.str);
				}
			}
			free(xl_oper->val.array.lparray);
		}

		free(xl_oper);
	}

	extern "C" __declspec(dllexport)
	int WINAPI xlAutoRemove(void)
	{
		/* Display a dialog box indicating that the XLL was successfully removed */
		Excel12f(xlcAlert, 0, 2, TempStr12(L"You have disabled oxl.xll oxl functions will no longer be available!"), TempInt12(2));
		return 1;
	}
}