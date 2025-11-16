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
#include "oxl/xl_api/xl_dictionary.h"
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
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlIsHolidayDay(LPXLOPER12 Date_input, LPXLOPER12 Centers_input)
{ 
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
 
  try
  {
       auto intermediate_result = oxl::OxlIsHolDay( Date_input,Centers_input );
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
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlComputeDayCount(LPXLOPER12 Start_Date_input, LPXLOPER12 End_Date_input, LPXLOPER12 Day_Count_Convention_input)
{ 
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
 
  try
  {
       auto intermediate_result = oxl::OxlComputeDayCount( Start_Date_input,End_Date_input,Day_Count_Convention_input );
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
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlComputeYearFraction(LPXLOPER12 Start_Date_input, LPXLOPER12 End_Date_input, LPXLOPER12 Day_Count_Convention_input)
{ 
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
 
  try
  {
       auto intermediate_result = oxl::OxlComputeYearFraction( Start_Date_input,End_Date_input,Day_Count_Convention_input );
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
 
extern "C" __declspec(dllexport) 
LPXLOPER12  oxlComputeDate(LPXLOPER12 Base_Date_input, LPXLOPER12 Tenor_input, LPXLOPER12 Adjustment_Rule_input, LPXLOPER12 Calendar_input)
{
  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));
  try
 {
    oxl::xl_api::XlDictionary dictionary_input;
    if (oxl::xl_api::XLoperObj::IsMulti(Base_Date_input ) && (oxl::xl_api::XLoperObj::IsMulti(Tenor_input )))
    {
         dictionary_input = oxl::xl_api::XLoperObj::LPXloperToDictionary(Base_Date_input, Tenor_input);
    }
    else if (oxl::xl_api::XLoperObj::IsMulti(Base_Date_input))
    {
         dictionary_input = oxl::xl_api::XLoperObj::LPXloperToDictionary(Base_Date_input);
    }
    else
    {
         if(oxl::xl_api::XLoperObj::IsStr(Base_Date_input))
         {
            std::string handle_str = oxl::xl_api::XLoperObj::LPXloperToStr(Base_Date_input);
            std::string key = oxl::xl_api::XlCacheObj::GetKeyFromHandle(handle_str);
            if(oxl::xl_api::XlCacheObj::IsDictionary(key))
            {
              auto cache_variant = oxl::xl_api::XlCacheObj::GetVariant(key);
              dictionary_input = *std::get<std::shared_ptr<oxl::xl_api::XlDictionary>>(cache_variant);
            }
         }
         else
         {
           dictionary_input["Base_Date"] = oxl::xl_api::XLoperObj::LPXloperToDouble(Base_Date_input); 
           dictionary_input["Tenor"] = oxl::xl_api::XLoperObj::LPXloperToStr(Tenor_input); 
           dictionary_input["Adjustment_Rule"] = oxl::xl_api::XLoperObj::LPXloperToStr(Adjustment_Rule_input); 
           dictionary_input["Calendar"] = oxl::xl_api::XLoperObj::LPXloperToStr(Calendar_input); 

         }
    }
       auto intermediate_result = oxl::OxlComputeDate(dictionary_input);
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
 
