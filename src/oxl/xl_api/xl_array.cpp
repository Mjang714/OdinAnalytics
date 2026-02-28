#include "xl_array.h"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "xl_variant.h"

namespace oxl::xl_api
{

////////////////////////////////////////////////////////////////////////////////
// XlArray::RowView                                                           //
////////////////////////////////////////////////////////////////////////////////

XlArray::RowView::RowView(RowData& data) noexcept : data_{&data} {}

std::size_t
XlArray::RowView::size() const noexcept
{
	return data_->size();
}

XlArray::RowView::Iter
XlArray::RowView::begin() const noexcept
{
	return data_->begin();
}

XlArray::RowView::Iter
XlArray::RowView::end() const noexcept
{
	return data_->end();
}

XlVariant&
XlArray::RowView::operator[](std::size_t i) const
{
	return data_->at(i);
}

bool
XlArray::RowView::operator==(RowView view) const
{
	// delegate to std::vector<T>::operator==
	return *data_ == *view.data_;
}

bool
XlArray::RowView::operator==(CRowView view) const
{
	// per-element check
	return std::ranges::equal(*this, view);
}

XlArray::RowView&
XlArray::RowView::operator=(RowData data)
{
	// size mismatch
	if (data.size() != data_->size())
		throw std::invalid_argument{
			"new row data size " + std::to_string(data.size()) +
			" != expected row data size " + std::to_string(data_->size())
		};
	// otherwise, move-assign
	*data_ = std::move(data);
	return *this;
}

const XlArray::RowData&
XlArray::RowView::operator*() const noexcept
{
	return *data_;
}

////////////////////////////////////////////////////////////////////////////////
// XlArray::CRowView                                                          //
////////////////////////////////////////////////////////////////////////////////

XlArray::CRowView::CRowView(const RowData& data) noexcept : data_{&data} {}

std::size_t
XlArray::CRowView::size() const noexcept
{
	return data_->size();
}

XlArray::CRowView::Iter
XlArray::CRowView::begin() const noexcept
{
	return data_->begin();
}

XlArray::CRowView::Iter
XlArray::CRowView::end() const noexcept
{
	return data_->end();
}

const XlVariant&
XlArray::CRowView::operator[](std::size_t i) const
{
	return data_->at(i);
}

bool
XlArray::CRowView::operator==(CRowView view) const
{
	// delegate to std::vector<T>::operator==
	return *data_ == *view.data_;
}

bool
XlArray::CRowView::operator==(RowView view) const
{
	// per-element check
	return std::ranges::equal(*this, view);
}

const XlArray::RowData&
XlArray::CRowView::operator*() const noexcept
{
	return *data_;
}

////////////////////////////////////////////////////////////////////////////////
// XlArray                                                                    //
////////////////////////////////////////////////////////////////////////////////

	XlArray::XlArray(size_t rows, size_t cols)
	  : m_rows_(rows), m_cols_(cols), m_data_(rows, RowData(cols))
	{

	}

	XlArray::XlArray(std::initializer_list<std::initializer_list<XlVariant>> data)
	{
		// allow empty array
		if (!data.size())
			return;
		// otherwise, set dimensions
		m_rows_ = data.size();
		m_cols_ = data.begin()->size();
		// current row index
		// set values with dimension checking
		std::size_t i = 0u;
		for (auto row : data) {
			// disallow ragged arrays
			if (row.size() != m_cols_)
				throw std::invalid_argument{
					"row " + std::to_string(i) + " has size " +
					std::to_string(row.size()) + " != expected row size " +
					std::to_string(m_cols_)
				};
			// copy values + advance
			m_data_.emplace_back(row);
			i++;
		}
	}

	XlArray::RowView
	XlArray::operator[](size_t index)
	{
		return m_data_.at(index);
	}

	XlArray::CRowView
	XlArray::operator[](const size_t index) const
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