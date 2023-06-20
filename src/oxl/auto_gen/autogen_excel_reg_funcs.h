// This is the registration funcition header that excel will use expose the function outwards.
// This auto generated header registration file that leverages the yaml file.

#include <format>
#include <stdlib.h>
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include <string>
#include "xlcall.h"
#include "framewrk.h"
#define excel_base_num_reg_functions_rows 3

static const WCHAR* excel_base_ngFuncs[excel_base_num_reg_functions_rows][12] = {
{ L"oxlArray", L"QQ", L"oxlArray", L"Data", L"1",L"Oxl - Excel - Tools",L"",L"",L"Creates a Array of the data from a given range",},
{ L"oxlDictionary", L"QQQ", L"oxlDictionary", L"Keys,Value", L"1",L"Oxl - Excel - Tools",L"",L"",L"Creates a Dictionary of the data from a given range",},
{ L"oxlDisplay", L"QQ", L"oxlDisplay", L"Handle", L"1",L"Oxl - Excel - Tools",L"",L"",L"Takes in the string handle and returns the cached array or dictionary data",},
};
