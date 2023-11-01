// This is the registration function header that excel will use expose the function outwards.
// This auto generated header registration file that leverages the yaml file.
  
#include <format>
#include <stdlib.h>
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include <string>
#include "xlcall.h"
#include "framewrk.h"
#define time_num_reg_functions_rows 5
 
static const WCHAR* time_ngFuncs[time_num_reg_functions_rows][12] = {
{ L"oxlIsBusinessDay", L"QQQ", L"oxlIsBusinessDay", L"Date,Centers", L"1",L"oxl_time",L"",L"",L"Determines if a date given is a business day",},
{ L"oxlIsHolidayDay", L"QQQ", L"oxlIsHolidayDay", L"Date,Centers", L"1",L"oxl_time",L"",L"",L"Determines if a date given is a Holiday day",},
{ L"oxlComputeDayCount", L"QQQQ", L"oxlComputeDayCount", L"Start_Date,End_Date,Day_Count_Convention", L"1",L"oxl_time",L"",L"",L"compute the number of business days",},
{ L"oxlComputeYearFraction", L"QQQQ", L"oxlComputeYearFraction", L"Start_Date,End_Date,Day_Count_Convention", L"1",L"oxl_time",L"",L"",L"compute the year fractions",},
{ L"oxlComputeDate", L"QQQQQ", L"oxlComputeDate", L"Base_Date,Tenor,Adjustment_Rule,Calendar", L"1",L"oxl_time",L"",L"",L"Computes a new date from date, tenor, calendar, date modifier",},
};
