#include "xl_dictionary.h"

#include <cstddef>
#include <format>
#include <initializer_list>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "xl_variant.h"

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
		)
			m_dict_[*k_it] = *v_it;
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
		)
			m_dict_[std::get<std::string>(*k_it)] = *v_it;
	}

	XlDictionary::XlDictionary(std::initializer_list<PairType> pairs)
	{
		for (const auto& [key, value] : pairs)
			m_dict_[key] = value;
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
		return m_dict_[key];
	}

	const XlVariant& XlDictionary::operator[] (const std::string& key) const
	{
		return  m_dict_.at(key);
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

	void
	XlDictionary::check_sizes(std::size_t n_keys, std::size_t n_values) const
	{
		// size check
		if (n_keys != n_values)
			throw std::invalid_argument{
				std::format(
					"{}:{}:{}: number of keys {} != number of values {}",
					__FILE__, __LINE__, __func__, n_keys, n_values
				)
			};
	}
}
