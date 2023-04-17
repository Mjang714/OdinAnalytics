#include "xl_array.h"

namespace oxl::xl_api
{
	XlArray::XlArray(size_t rows, size_t cols) : m_rows_(rows), m_cols_(cols), m_data_(rows,std::vector<XlVariant>(cols))
	{

	}

	std::vector<XlVariant>& XlArray::operator[] (size_t index)
	{
		return m_data_.at(index);
	}

	const std::vector<XlVariant>& XlArray::operator[] (const size_t index) const
	{
		return m_data_.at(index);
	}

	XlVariant& XlArray::operator() (size_t row, size_t cols)
	{
		return m_data_.at(row).at(cols);
	}

	const XlVariant& XlArray::operator() (const size_t row, const size_t cols) const
	{
		return m_data_.at(row).at(cols);
	}

	std::vector<XlVariant> XlArray::ToVector()
	{
		if ((m_rows_ != 1) && (m_cols_ != 1))
		{
			throw "xl_array.cpp line 34 ToVector(): Not a Valid Array must be 1xN or Nx1";
		}

		std::vector<XlVariant> list_of_values;
		if (m_cols_ == 1)
		{
			for (size_t i = 0; i < m_rows_; i++)
			{
				list_of_values.push_back(m_data_.at(i).at(0));
			}
		}

		else
		{
			for (size_t j = 0; j < m_cols_; j++)
			{
				list_of_values.push_back(m_data_.at(0).at(j));
			}
		}

		return list_of_values;
	}
}