#ifndef OXL_XL_API_XL_DICTIONARY_H_
#define OXL_XL_API_XL_DICTIONARY_H_

#include <cstddef>
#include <initializer_list>
#include <map>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "oxl/xl_api/xl_variant.h"

namespace oxl::xl_api
{
	/**
	 * `XlVariant` key-value mapping type.
	 *
	 * This is an intermediary class that assists in converting data between
	 * the C++ and Excel layers via the `XLOperObj` members. In particular,
	 * this wraps a `std::map` that maps string names to `XlVariant` values.
	 *
	 * Construction from spans of keys and values and list-init are supported.
	 */
	class XlDictionary
	{
	public:
		using MapType = std::map<std::string, XlVariant>;
		using PairType = MapType::value_type;
		using Iter = MapType::iterator;
		using CIter = MapType::const_iterator;
		// input spans for string keys and variant values
		using KeySpan = std::span<const std::string>;
		using ValueSpan = std::span<const XlVariant>;

		/**
		 * Default ctor.
		 */
		XlDictionary() = default;

		/**
		 * Ctor.
		 *
		 * Construct from a span of keys and a span of values. Keys must be
		 * non-empty string with no leading or trailing whitespace.
		 *
		 * @param keys Dictionary keys
		 * @param values Dictionary values
		 */
		XlDictionary(KeySpan keys, ValueSpan values);

		/**
		 * Ctor.
		 *
		 * Construct from a span of keys and a span of values. Keys must be
		 * non-empty strings with no leading or trailing whitespace.
		 *
		 * @param keys Dictionary keys
		 * @param values Dictionary values
		 */
		XlDictionary(ValueSpan keys, ValueSpan values);

		/**
		 * Ctor.
		 *
		 * Construct from pairs of string keys and values. For example:
		 *
		 * @code{.cc}
		 * XlDictionary dict{
		 *   {"hello", 2.},
		 *   {"world", true},
		 *   {"sky", "blue"}
		 * };
		 * @endcode
		 *
		 * Keys cannot be empty or contain leading or trailing whitespace.
		 *
		 * @param pairs Initializer list of key-value pairs
		 */
		XlDictionary(std::initializer_list<PairType> pairs);

		/**
		 * Return the size of the dictionary.
		 */
		std::size_t size() const noexcept;

		/**
		 * Return a read-only iterator to the first mapping pair.
		 */
		CIter begin() const noexcept;

		/**
		 * Return an iterator to the first mapping pair.
		 *
		 * The mapped value can be modified through the iterator.
		 */
		Iter begin() noexcept;

		/**
		 * Return a read-only iterator one past the last mapping pair.
		 */
		CIter end() const noexcept;

		/**
		 * Return an iterator one past the last mapping pair.
		 */
		Iter end() noexcept;

		/// <summary>
		/// return XlVariant that is mutable and is associated with the key
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		XlVariant& operator[](const std::string& key);

		/// <summary>
		/// returns a XlVariant that is immutable and is associated with the given key
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		const XlVariant& operator[](const std::string& key) const;

		/// <summary>
		/// return wether a value exist within the underlying map
		/// </summary>
		/// <param name="key"></param>
		/// <returns>bool</returns>
		bool Contains(const std::string& key) const;

		// TODO: deprecated as iterators should be preferred
		std::vector<std::pair<std::string, XlVariant>> GetKeyValuePair() const;

		void ApplyOverrides(const XlDictionary& overrides_dict);

		bool IsEmpty() const;

	private:
		MapType m_dict_;

		// ensures key and value ranges are the same size
		void check_sizes(std::size_t n_keys, std::size_t n_values) const;

		// ensures keys are not empty and have no leading/trailing blanks
		static void check_key(const std::string& key);
	};
}

#endif // !OXL_XL_API_XL_DICTIONARY_H_
