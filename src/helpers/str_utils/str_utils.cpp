#include "str_utils.h"

namespace oa::utils::str_utils
{
	bool IsDigit(const std::string& str_to_parse) noexcept
	{
		for (auto char_int : str_to_parse)
		{
			if (!std::isdigit(char_int))
			{
				return false;
			}
		}
		return true;
	}

	std::vector<int> StrToDigits(const std::string& str_to_parse) noexcept
	{
		std::vector<int> digits;
		if (IsDigit(str_to_parse))
		{
			for (auto digit_char : str_to_parse)
			{
				digits.push_back(digit_char - '0');
			}
		}

		return digits;
	}
}