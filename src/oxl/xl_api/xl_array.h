#ifndef OXL_XL_API_XL_ARRAY_H_
#define OXL_XL_API_XL_ARRAY_H_

#include <cstddef>
#include <initializer_list>
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
	 *
	 * The row data itself can be copy-assigned from `operator*`:
	 *
	 * @code{.cc}
	 * // assuming arr is an XlArray shape (4, 4)
	 * auto row = *arr[0];
	 * // row is unchanged after assigning values
	 * arr[0] = {2., std::string{"a"}, false, 50.};
	 * assert(row[1] != arr[0][1]);
	 * @endcode
	 */
	class XlArray
	{
	public:
		// row data type
		using RowData = std::vector<XlVariant>;

		// forward decl for RowView::operator==
		class CRowView;

		/**
		 * Modifiable view of the `XlArray` row data.
		 */
		class RowView {
		public:
			using Iter = RowData::iterator;
			using value_type = XlVariant;    // EXPECT_THAT() compatibility

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
			Iter begin() const noexcept;

			/**
			 * Return an iterator one past the last row element.
			 */
			Iter end() const noexcept;

			/**
			 * Return a reference to the specified row element.
			 *
			 * Indexing out of bounds will throw an exception.
			 *
			 * @param i Element index
			 */
			XlVariant& operator[](std::size_t i) const;

			/**
			 * Compare row elements for equality.
			 *
			 * @param view Row view
			 */
			bool operator==(RowView view) const;

			/**
			 * Compare row elements for equality.
			 *
			 * @param view Row view
			 */
			bool operator==(CRowView view) const;

			/**
			 * Return a const reference to the row data.
			 *
			 * This is useful when a direct reference to the row data is needed
			 * but is mostly intended to enable succinct copying.
			 */
			const RowData& operator*() const noexcept;

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
			using value_type = XlVariant;    // EXPECT_THAT() compatibility

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

			/**
			 * Compare row elements for equality.
			 *
			 * @param view Row view
			 */
			bool operator==(CRowView view) const;

			/**
			 * Compare row elements for equality.
			 *
			 * @param view Row view
			 */
			bool operator==(RowView view) const;

			/**
			 * Return a const reference to the row data.
			 *
			 * This is useful when a direct reference to the row data is needed
			 * but is mostly intended to enable succinct copying.
			 */
			const RowData& operator*() const noexcept;

		private:
			const RowData* data_;  // trivially copyable
		};

		/**
		 * Ctor.
		 *
		 * This initializes an `XlArray` of the given dimensions with
		 * value-initialized `XlVariant` values. Therefore, each `XlVariant`
		 * will contain a value-initialized value of its first alternative.
		 *
		 * @param rows Number of rows
		 * @param cols Number of columns
		 */
		XlArray(size_t rows, size_t cols);

		/**
		 * Ctor.
		 *
		 * Constructs from a non-ragged list of initializer lists, e.g.
		 *
		 * @code{.cc}
		 * XlArray arr{
		 *   {1., std::string{"hello"}, true},
		 *   {2., std::string{"world"}, false}
		 *   {true, 3., std::string{"beta"}}
		 * };
		 * @endcode
		 *
		 * @param data Nested init-lists of elements
		 */
		XlArray(std::initializer_list<std::initializer_list<XlVariant>> data);

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

		// TODO: can add ref-qualified overload that moves values
		std::vector<XlVariant> ToVector() const;

		size_t rows() const { return m_rows_; };
		size_t cols() const { return m_cols_; };

	private:
		size_t m_rows_{};
		size_t m_cols_{};
		std::vector<RowData> m_data_;
	};

}  // namespace oxl::xl_api

#endif //OXL_XL_API_XL_ARRAY_H_
