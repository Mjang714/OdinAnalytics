#ifndef OXL_XL_API_XL_DICTIONARY_H_
#define OXL_XL_API_XL_DICTIONARY_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "oxl/xl_api/xl_variant.h"

namespace oxl::xl_api
{
	class XlDictionary
	{
		public:
			XlDictionary() = default;

			XlDictionary(std::vector<XlVariant> keys, std::vector<XlVariant> values);

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

			std::vector<std::pair<std::string, XlVariant>> GetKeyValuePair(void) const;

			void ApplyOverrides(const XlDictionary& overrides_dict);


			bool IsEmpty() const;
		private:
			/// @brief A function to check if there are any trialing white spaces in the key as this can cause confusion for users when trying to access values in the dictionary
			/// @param key string_view of the key to be checked
			/// @return void but will throw an exception if the key is invalid
			static void checkKeyValid(const std::string& key);
			std::map<std::string, XlVariant> m_dict_;
			std::map<std::string, XlVariant> dict(void) { return m_dict_; };
			std::map<std::string, XlVariant> dict(void) const { return m_dict_; };
	};
}

#endif // !OXL_XL_API_XL_DICTIONARY_H_

