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
#include "oxl/excel_base_funcs.h"
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlArray(LPXLOPER12 Data_input)
{ 
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
 
  try
   {
       oxl::xl_api::CachedObjVar cache_obj = oxl::OxlCreateArray( Data_input );
       std::string base_name = oxl::xl_api::XlCacheObj::GenHandleStr(cache_obj);
       auto cached_obj = oxl::xl_api::XlCacheObj(cache_obj, base_name);
       oxl::xl_api::XLoperObj::ConvertToLPXloper(cached_obj.CacheName(), xloper_result);
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
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlDictionary(LPXLOPER12 Keys_input, LPXLOPER12 Value_input)
{ 
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
 
  try
   {
       oxl::xl_api::CachedObjVar cache_obj = oxl::OxlCreateDict( Keys_input,Value_input );
       std::string base_name = oxl::xl_api::XlCacheObj::GenHandleStr(cache_obj);
       auto cached_obj = oxl::xl_api::XlCacheObj(cache_obj, base_name);
       oxl::xl_api::XLoperObj::ConvertToLPXloper(cached_obj.CacheName(), xloper_result);
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
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlDisplay(LPXLOPER12 Handle_input)
{ 
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
 
  try
   {
       auto intermediate_result = oxl::OxlDisplay( Handle_input );
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
 
