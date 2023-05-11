// This is the registration funcition cpp that excel will use expose the function outwards.
// This auto generated cpp registration file that leverages the yaml file.
  
#include <format>
#include <stdlib.h>
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include <string>
#include "xlcall.h"
#include "framewrk.h"
#include "oxl/xl_api/xloper_converter.h"
#include "oxl/xl_api/cache_xl_obj.h"
#include "oxl/time_xl.h"
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlIsBusinessDay(LPXLOPER12 Date_input, LPXLOPER12 Centers_input)
{ 
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
 
  try
  {
       auto intermediate_result = oxl::OxlIsBizDay( Date_input,Centers_input );
       oxl::xl_api::XLoperObj::ConvertToLPXloper(intermediate_result, xloper_result);
       return xloper_result;
   }
 
   catch(std::exception& e)
   {
       oxl::xl_api::XLoperObj::ConvertToLPXloper("ERROR:" + std::string(e.what()), xloper_result);
       return xloper_result;
   }

   catch(...)
   {
       xloper_result->xltype = xltypeErr;
       xloper_result->val.err = xlerrValue;
       return xloper_result;
   }
}
