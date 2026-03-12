#include "xl_dictionary.h"

#include <cctype>
#include <cstddef>
#include <format>
#include <initializer_list>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <format>

#include "xl_variant.h"
#include "oa/ctti.h"

namespace oxl::xl_api
{
	XlDictionary::XlDictionary(KeySpan keys, ValueSpan values)
	{
		// size check
		check_sizes(keys.size(), values.size());
		// insert
		// note: could weaken inputs to forward ranges instead
		for (
			auto [k_it, v_it] = std::pair{keys.begin(), values.begin()};
			k_it != keys.end();
			k_it++, v_it++
		) {
			check_key(*k_it);
			m_dict_[*k_it] = *v_it;
		}
	}

	XlDictionary::XlDictionary(ValueSpan keys, ValueSpan values)
	{
		// size check
		check_sizes(keys.size(), values.size());
		// insert
		for (
			auto [k_it, v_it] = std::pair{keys.begin(), values.begin()};
			k_it != keys.end();
			k_it++, v_it++
		) {
			auto key = std::get<std::string>(*k_it);
			check_key(key);
			m_dict_[key] = *v_it;
		}
	}

	XlDictionary::XlDictionary(std::initializer_list<PairType> pairs)
	{
		for (const auto& [key, value] : pairs) {
			check_key(key);
			m_dict_[key] = value;
		}
	}

	std::size_t
	XlDictionary::size() const noexcept
	{
		return m_dict_.size();
	}

	XlDictionary::Iter
	XlDictionary::begin() noexcept
	{
		return m_dict_.begin();
	}

	XlDictionary::CIter
	XlDictionary::begin() const noexcept
	{
		return m_dict_.begin();
	}

	XlDictionary::Iter
	XlDictionary::end() noexcept
	{
		return m_dict_.end();
	}

	XlDictionary::CIter
	XlDictionary::end() const noexcept
	{
		return m_dict_.end();
	}

	XlVariant& XlDictionary::operator[] (const std::string& key)
	{
		// note: since value might be default-inserted we need a check
		check_key(key);
		return m_dict_[key];
	}

	const XlVariant& XlDictionary::operator[] (const std::string& key) const
	{
		// note: no key validity check since at() will throw
		return m_dict_.at(key);
	}

	bool XlDictionary::Contains(const std::string& key) const
	{
		return m_dict_.contains(key);
	}

	std::vector<std::pair<std::string, XlVariant>> XlDictionary::GetKeyValuePair(void) const
	{
		std::vector<std::pair<std::string, XlVariant>> key_value_pair_list;

		for (const auto& key_value_pair : m_dict_)
		{
			key_value_pair_list.push_back(key_value_pair);
		}

		return key_value_pair_list;
	}

	void XlDictionary::ApplyOverrides(const XlDictionary& overrides_dict)
	{
		for (const auto& [key, value] : overrides_dict)
			m_dict_[key] = value;
	}

	bool XlDictionary::IsEmpty() const
	{
		return m_dict_.empty();
	}

	void
	XlDictionary::check_sizes(std::size_t n_keys, std::size_t n_values) const
	{
		// size check
		if (n_keys != n_values)
			throw std::invalid_argument(std::format("{}:{}: number of keys {} != number of values {}", std::string{OA_SOURCE_LOCATION()}, std::string{__func__}, 
				n_keys, n_values));
	}

	void XlDictionary::check_key(const std::string& key)
	{
		// key cannot be empty either
		if (key.empty())
			throw std::invalid_argument{"empty string not allowed as key"};
		// no leading or trailing spaces
		if (std::isspace(key.front()) || std::isspace(key.back()))
			throw std::invalid_argument{
				std::format(
					"{}:{}:"	
					"key \"{}\" invalid: "
					"keys cannot contain leading or trailing spaces", 
					std::string{OA_SOURCE_LOCATION()}, std::string{__func__}, key
				)
			};
	}
}
