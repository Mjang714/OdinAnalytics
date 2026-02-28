#ifndef OXL_XL_API_XL_ARRAY_H_
#define OXL_XL_API_XL_ARRAY_H_

#include <cstddef>
#include <vector>

#include "xl_variant.h"

namespace oxl::xl_api
{

	/**
	 * `XlVariant` array type.
	 *
	 * This is an intermediary class that assists in converting data between
	 * the C++ and Excel layers via the `XLOperObj` members. To set and view
	 * entire `XlArray` rows, use the `operator[]` overloads:
	 *
	 * @code{.cc}
	 * // assuming arr is an XlArray shape (4, 4)
	 * arr[0] = {1., std::string{"hello"}, true, 100.};
	 * @endcode
	 *
	 * Rows can also be iterated through like a normal random-access container:
	 *
	 * @code{.cc}
	 * // assuming arr is an XlArray shape (4, 4)
	 * for (auto& value : arr[0])
	 *   ;
	 * @endcode
	 */
	class XlArray
	{
	public:
		// row data type
		using RowData = std::vector<XlVariant>;

		/**
		 * Modifiable view of the `XlArray` row data.
		 */
		class RowView {
		public:
			using Iter = RowData::iterator;

			/**
			 * Ctor.
			 *
			 * @param data Row data
			 */
			RowView(RowData& data) noexcept;

			/**
			 * Return the number of elements in the row.
			 */
			std::size_t size() const noexcept;

			/**
			 * Return an iterator to the first row element.
			 */
			Iter begin() noexcept;

			/**
			 * Return an iterator one past the last row element.
			 */
			Iter end() noexcept;

			/**
			 * Return a reference to the specified row element.
			 *
			 * Indexing out of bounds will throw an exception.
			 *
			 * @param i Element index
			 */
			XlVariant& operator[](std::size_t i);

			/**
			 * Assign new row data.
			 *
			 * If the new data dimensions don't match an exception is thrown.
			 *
			 * @param data New row data
			 */
			RowView& operator=(RowData data);

		private:
			RowData* data_;  // trivially copyable
		};

		/**
		 * Non-modifiable view of the `XlArray` row data.
		 */
		class CRowView {
		public:
			using Iter = RowData::const_iterator;

			/**
			 * Ctor.
			 *
			 * @param data Row data
			 */
			CRowView(const RowData& data) noexcept;

			/**
			 * Return the number of elements in the row.
			 */
			std::size_t size() const noexcept;

			/**
			 * Return an iterator to the first const-qualified row element.
			 */
			Iter begin() const noexcept;

			/**
			 * Return an iterator one past the last const-qualified row element.
			 */
			Iter end() const noexcept;

			/**
			 * Return a const reference to the specified row element.
			 *
			 * Indexing out of bounds will throw an exception.
			 *
			 * @param i Element index
			 */
			const XlVariant& operator[](std::size_t i) const;

		private:
			const RowData* data_;  // trivially copyable
		};

		/**
		 * Ctor.
		 *
		 * @param rows Number of rows
		 * @param cols Number of columns
		 */
		XlArray(size_t rows, size_t cols);

		/**
		 * Returns a modifiable view of the specified row.
		 *
		 * @param i Row index
		 */
		RowView operator[](size_t i);

		/**
		 * Returns a non-modifiable view of the specified row.
		 *
		 * @param i Row index
		 */
		CRowView operator[](const size_t i) const;

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
		std::vector<RowData> m_data_;
	};

}  // namespace oxl::xl_api

#endif //OXL_XL_API_XL_ARRAY_H_
