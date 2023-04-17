#include "excel_base.h"



namespace oxl::xl_api
{
	std::shared_ptr<XlArray> ToXLArrayPtr(const LPXLOPER12& data_range)
	{
		XlArray xl_array = XLoperObj::LPXloperToXlArray(data_range);
		return std::shared_ptr<XlArray>(new XlArray(xl_array));
	}
}