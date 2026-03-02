#include "xl_dictionary.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <format>

#include "xl_variant.h"

namespace oxl::xl_api
{
	XlDictionary::XlDictionary(std::vector<XlVariant> keys, std::vector<XlVariant> values)
	{
		if (keys.size() != values.size())
		{
			throw std::invalid_argument("XlDictionary.cpp line 10 XlDictionary(vector, vector):Key and Value list does not match");
		}

		for (size_t i = 0; i < keys.size(); i++)
		{
			std::string key = std::get<std::string>(keys.at(i));
			if(!IsKeyValid(key))
			{	
				throw std::invalid_argument(std::format("{}:{}","XlDictionary(vector, vector):Key cannot end or begin with empty space", "\"" + key + "\""));
			}
			m_dict_[key] = values[i];
		}
	}

	XlVariant& XlDictionary::operator[] (const std::string& key)
	{

		return m_dict_[key];
	}

	const XlVariant& XlDictionary::operator[] (const std::string& key) const
	{
		if(!IsKeyValid(key))
		{
			throw std::invalid_argument(std::format("{}:{}","XlDictionary[] (str):Key cannot end or begin with empty space", "\"" + key + "\""));
		}
		return  m_dict_.at(key);
	}

	bool XlDictionary::Contains(const std::string& key) const
	{
		if(!IsKeyValid(key))
		{
			throw std::invalid_argument(std::format("{}:{}","Contains(str) Key cannot end or begin with empty space", "\"" + key + "\""));
		}
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
		//when applying overides use structured bindings
		for (const auto &[key, value] : overrides_dict.GetKeyValuePair())
		{
			m_dict_[key] = value;
		}
	}

	bool XlDictionary::IsEmpty() const 
	{
		return !this->GetKeyValuePair().size();
	}

	bool XlDictionary::IsKeyValid(const std::string_view& key)
	{
		return !std::isspace(key.back()) && !std::isspace(key.front());
	}
}