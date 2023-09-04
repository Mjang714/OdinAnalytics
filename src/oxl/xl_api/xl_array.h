#ifndef OXL_XL_API_XL_ARRAY_H_
#define OXL_XL_API_XL_ARRAY_H_

#include <vector>

#include "xl_variant.h"

namespace oxl::xl_api
{
	/// <summary>
	/// this is an intermediary class that converts data from c++ to excel layer via the XLOperObj
	/// </summary>
	class XlArray
	{
		public:
			XlArray(size_t rows, size_t cols);
			/// <summary>
			/// returns an entire row of the array
			/// </summary>
			/// <param name="i"></param>
			/// <returns></returns>
			std::vector<XlVariant>& operator [](size_t i);

			/// <summary>
			/// this is const method to return an entire row
			/// </summary>
			/// <param name="i"></param>
			/// <returns></returns>
			const std::vector<XlVariant>& operator [](const size_t i) const;

			/// <summary>
			/// operation to retrieve an element of an array that is mutable
			/// </summary>
			/// <param name="row"></param>
			/// <param name="col"></param>
			/// <returns></returns>
			XlVariant& operator () (size_t row,  size_t col);

			/// <summary>
			/// operation to retrieve an element of an array that is inmutable
			/// </summary>
			/// <param name="row"></param>
			/// <param name="col"></param>
			/// <returns></returns>
			const XlVariant& operator () (const size_t row, const size_t col) const;

			std::vector<XlVariant> ToVector();

			size_t rows() const { return m_rows_; };
			size_t cols() const { return m_cols_; };

		private:
			size_t m_rows_;
			size_t m_cols_;
			std::vector<std::vector<XlVariant>> m_data_;
	};
}



#endif //OXL_XL_API_XL_ARRAY_H_
