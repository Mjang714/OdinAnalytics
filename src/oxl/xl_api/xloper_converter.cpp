#include "xloper_converter.h"


namespace oxl::xl_api {

	XLoperObj::XLoperObj(const double input)
	{
		m_data_ = input;
	}

	XLoperObj::XLoperObj(const char* input)
	{
		m_data_ = std::string(input);
	}

	XLoperObj::XLoperObj(const LPXLOPER12 input)
	{
		m_data_ = input;
	}

	bool XLoperObj::IsEmpty(const LPXLOPER12 input)
	{
		if (input->xltype == xltypeMissing || input->xltype == xltypeNil)
		{
			return true;
		}

		//if it is not nil then we can return true
		return false;
	}

	bool XLoperObj::IsMulti(const LPXLOPER12 input)
	{
		if (input->xltype == xltypeMulti)
		{
			return true;
		}

		return false;
	}

	bool XLoperObj::HasData(void)
	{
		if (std::holds_alternative<double>(m_data_))
		{
			return true;
		}

		else if (std::holds_alternative<bool>(m_data_))
		{
			return true;
		}

		else if (std::holds_alternative<std::string>(m_data_))
		{
			return true;
		}
		else if (std::holds_alternative<LPXLOPER12>(m_data_))
		{
			LPXLOPER12 var_data = std::get<LPXLOPER12>(m_data_);
			//check if it is a empty XLOPER and if it is not then return true
			if (!IsEmpty(var_data))
			{
				return true;
			}

			return false;
		}
		else
		{
			return false;
		}
		
	}

	void XLoperObj::ConvertToLPXloper(double value, LPXLOPER12 xl_oper_res)
	{
		xl_oper_res->xltype = xltypeNum;
		xl_oper_res->val.num = value;
	}

	void XLoperObj::ConvertToLPXloper(bool value, LPXLOPER12 xl_oper_res)
	{
		
		xl_oper_res->xltype = xltypeBool;
		xl_oper_res->val.xbool = value;
	
	}

	void XLoperObj::ConvertToLPXloper(const std::string& value, LPXLOPER12 xl_oper_res)
	{	

		// Convert to wide string
		std::wstring wide_str = ToWString(value);

		//declare the xloper12 type as str and DLL free bit that will be freed later on 
		xl_oper_res->xltype = xltypeStr | xlbitDLLFree;
		//allocate the memory size of the wide char array +2 bytes wide character string are not the same size as 
		// normal strings
		xl_oper_res->val.str = static_cast<XCHAR*> (calloc(wide_str.length() + 2, sizeof(XCHAR)));

		//the first wide character is the size of the wide char array
		xl_oper_res->val.str[0] = static_cast<XCHAR> (wide_str.length());

		
		for (size_t i = 0; i < wide_str.length(); i++)
		{
			// orignal line was check to see if this one works
			//xl_oper_res->val.str[i + 1] = wide_str.data()[i];
			xl_oper_res->val.str[i + 1] = wide_str[i];
		}

		xl_oper_res->val.str[value.length() + 1] = L'\0';
	}

	void XLoperObj::ConvertToLPXloper(const XlArray& value, LPXLOPER12 xl_oper_res)
	{
		size_t rows = value.rows();
		size_t cols = value.cols();

		xl_oper_res->xltype = xltypeMulti | xlbitDLLFree;
		xl_oper_res->val.array.rows = rows;
		xl_oper_res->val.array.columns = cols;
		xl_oper_res->val.array.lparray = static_cast<LPXLOPER12> (malloc((sizeof(XLOPER12) * rows * cols)));

		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < cols; j++)
			{
				if (std::holds_alternative <bool> (value(i,j)))
				{
					ConvertToLPXloper(std::get<bool>(value(i, j)), &xl_oper_res->val.array.lparray[i*cols +j]);
				}

				else if (std::holds_alternative <double>(value(i, j)))
				{
					ConvertToLPXloper(std::get<double>(value(i, j)), &xl_oper_res->val.array.lparray[i * cols + j]);
				}

				//return a string in the defualt case 
				else
				{
					ConvertToLPXloper(std::get<std::string>(value(i, j)), &xl_oper_res->val.array.lparray[i * cols + j]);
				}
			}
		}
	}


	XlArray XLoperObj::LPXloperToXlArray(const LPXLOPER12& xl_oper)
	{
		if (!IsMulti(xl_oper))
		{
			std::cout << "Not a xlTypeMulti please check input \n";
			throw std::invalid_argument("Input was not 2D array check the input:xloper_converter.cpp line 155 LPXloperToArray()");
		}
		
		int rows = xl_oper->val.array.rows;
		int cols = xl_oper->val.array.columns;

		XlArray xl_array_data(rows, cols);

		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < cols; j++)
			{
				
				if (xl_oper->val.array.lparray[(i * cols) + j].xltype == xltypeNum)
				{
					xl_array_data(i, j) = xl_oper->val.array.lparray[(i * cols) + j].val.num;
				}
				else if (xl_oper->val.array.lparray[(i * cols) + j].xltype == xltypeBool)
				{
					xl_array_data(i, j) = static_cast<bool> (xl_oper->val.array.lparray[(i * cols) + j].val.xbool);
				}
				else
				{
					//check to see if it is empty
					if ( xl_oper->val.array.lparray[(i * cols) + j].xltype != xlretInvAsynchronousContext)
					{
						xl_array_data(i, j) = LPXloperToStr(&xl_oper->val.array.lparray[(i * cols) + j]);
					}
					//if it is empty then use empty string
					else 
					{
						xl_array_data(i, j) = "";
					}
				}
			}
		}
		return xl_array_data;
	}

	XlDictionary XLoperObj::LPXloperToDictionary(const LPXLOPER12& xl_oper)
	{
		if (!IsMulti(xl_oper))
		{
			throw std::invalid_argument("Input was not xltypeMulti check the input: xloper_converter.cpp line 190 LPXloperToDictionary()");
		}
		
		XlArray xl_array = LPXloperToXlArray(xl_oper);

		return XlArrayToXlDictionary(xl_array);
	}

	XlDictionary XLoperObj::LPXloperToDictionary(const LPXLOPER12& xl_oper_keys, const LPXLOPER12& xl_oper_values)
	{
		XlArray xl_array_key = LPXloperToXlArray(xl_oper_keys);
		XlArray xl_array_values = LPXloperToXlArray(xl_oper_values);

		return XlDictionary(xl_array_key.ToVector(), xl_array_values.ToVector());
	}

	std::wstring XLoperObj::ToWString(const std::string& value)
	{
		size_t size = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), value.length(), NULL, 0);
		std::wstring wstr_value(size, 0);
		MultiByteToWideChar(CP_UTF8, 0, value.c_str(), value.length(), &wstr_value[0], size);
		return wstr_value;

	}

	std::string XLoperObj::ToUTF8String(const std::wstring& value)
	{
		size_t size = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), value.length(), NULL, 0, NULL, NULL);
		std::string utf8_str(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, &utf8_str[0], size, NULL, NULL);
		return utf8_str;
	}

	std::string XLoperObj::LPXloperToStr(const LPXLOPER12& xl_oper)
	{
		if (xl_oper->xltype != xltypeStr)
		{
			throw std::invalid_argument("Not an Excel String Type line check the input: xloper_converter.cpp line 209 LPXloperToStr()");
		}

		std::string str  = ToUTF8String(xl_oper->val.str + 1);
		return str.substr(0, static_cast<int> (xl_oper->val.str[0]));
	}

	std::string XLoperObj::CellName(void)
	{
		xloper12 cell_loc;
		xloper12 sheet_name;

		Excel12(xlfCaller, &cell_loc, 0);
		Excel12(xlSheetNm, &sheet_name,1, &cell_loc);

		std::string sheet_name_str = LPXloperToStr(&sheet_name);

		int row = cell_loc.val.sref.ref.rwFirst;
		int col = cell_loc.val.sref.ref.colFirst;

		return std::format("{}_${}${}", sheet_name_str, row, col);
	}
}