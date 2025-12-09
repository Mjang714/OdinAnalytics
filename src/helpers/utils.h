#ifndef ODINANALYTICS_HELPERS_UTILS_H_
#define	ODINANALYTICS_HELPERS_UTILS_H_

#include <regex>
#include <string>

// FIXME: once VS is fixed to correctly namespace we don't need this hackery
#if !__has_include(<magic_enum/magic_enum.hpp>)
// for VS compatibility
#include "magic_enum.hpp"
#else
// recommended Magic Enum usage
#include <magic_enum/magic_enum.hpp>
#endif  // __has_include(<magic_enum/magic_enum.hpp>)

/// <summary>
/// This library provides basic utilities that will be used else where to do basic things like check if we have valid tenor string
/// lcu = logic check utils
/// </summary>
namespace oa::utils
{
	/// <summary>
	/// This function returns a  boolean for a string if it is valid string type such as 12M, 3D, 6y,3w
	/// </summary>
	/// <param name="input_string"></param>
	/// <returns> boolean </returns>
	inline bool CheckTenorStr(const std::string& input_string)
	{
		std::regex target_str("([-]?[0-9]*\\d{1}[YMDWymdw])");
		return std::regex_match(input_string, target_str);
	}

	/// <summary>
	/// Valid Date string YYYY-MM-DD,YYYY/MM/DD, and YYYY:MM:DD
	/// </summary>
	/// <param name="input_string"></param>
	/// <returns>boolean</returns>
	inline bool CheckDateStr(const std::string& input_string)
	{
		std::regex target_str("(\\d{4}[\\-\\/:](0?[1-9]|1[012])[\\-\\/:](0?[1-9]|[12][0-9]|3[01]))");
		return std::regex_match(input_string, target_str);
	}

	/// <summary>
	/// converts an Enum to integer value
	/// </summary>
	/// <typeparam name="enum_type"></typeparam>
	/// <param name="given_enum"></param>
	/// <returns> an integer </returns>
	template <typename enum_type> requires std::is_enum_v<enum_type>
	inline int EnumToInt(enum_type given_enum)
	{
		return magic_enum::enum_integer(given_enum);
	}

	/// <summary>
	/// takes in an enum and converts it to its value
	/// </summary>
	/// <typeparam name="enum_type"></typeparam>
	/// <typeparam name="value_type"></typeparam>
	/// <param name="value"></param>
	/// <returns></returns>
	template <typename enum_type, typename value_type> requires std::is_enum_v<enum_type>
	inline enum_type ValueToEnum(value_type value)
	{
		auto result = magic_enum::enum_cast<enum_type>(value);
		if (result.has_value())
		{
			return result.value();
		}
		else
		{
			//TODO: throw out the enum_type name as a string
			throw "invalid enum please check the given enum ";
		}
	}

	/// <summary>
	/// returns the name of the enum name as a string from the value of the enum
	/// </summary>
	/// <typeparam name="enum_type"></typeparam>
	/// <param name="enum_value"></param>
	/// <returns>the string name of the enum</returns>
	template <typename enum_type> requires std::is_enum_v<enum_type>
	inline std::string GetEnumName(enum_type enum_value)
	{
		return static_cast<std::string> (magic_enum::enum_name(enum_value));
	}

	/// <summary>
	/// this function will return the enum name with the first char removed (which is k)
	/// </summary>
	/// <typeparam name="enum_type"></typeparam>
	/// <param name="enum_value"></param>
	/// <returns>a string of the enum with the first character removed</returns>
	template <typename enum_type> requires std::is_enum_v<enum_type>
	inline std::string GetCleanName(enum_type enum_value)
	{

		return GetEnumName(enum_value).erase(0, 1);
	}

}

#endif // ODINANALYTICS_HELPERS_UTILS_H_
