#include "excel_base.h"

#include "xl_array.h"
#include "xloper_converter.h"

namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const xloper12* data_range)
	{
		XlArray xl_array = XLoperObj::LPXloperToXlArray(data_range);
		return std::shared_ptr<XlArray>(new XlArray(xl_array));
	}
}