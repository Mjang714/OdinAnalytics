#ifndef OXL_XL_API_EXCEL_BASE_H_
#define OXL_XL_API_EXCEL_BASE_H_

#include <stdlib.h>
#define NOMINMAX
#include <Windows.h>
#include <string>
#include <memory>

#include "xlcall.h"
#include "framewrk.h"

#include "xloper_converter.h"
#include "xl_array.h"
#include "cache_xl_obj.h"

namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const LPXLOPER12& data_range);
}

#endif // !OXL_XL_API_EXCEL_BASE_H_
