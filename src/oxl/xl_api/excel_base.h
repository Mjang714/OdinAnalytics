#ifndef OXL_XL_API_EXCEL_BASE_H_
#define OXL_XL_API_EXCEL_BASE_H_

#include <memory>

#include "xl_array.h"

// forward decl to avoid bringing in XLCALL.H
struct xloper12;

namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const xloper12* data_range);
}

#endif // !OXL_XL_API_EXCEL_BASE_H_
