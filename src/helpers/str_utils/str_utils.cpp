#include "str_utils.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include "oa/string.h"

namespace oa::utils::str_utils {

// note: could generalize to any kind of character range
bool IsDigit(const std::string& str) noexcept
{
	for (auto c : str)
		if (!is_digit(c))
			return false;
	return true;
}

std::vector<int> StrToDigits(const std::string& str)
{
	// ensure str only contains digits
	if (!IsDigit(str))
		throw std::runtime_error{"input string contains non-digit characters"};
	// otherwise convert to int
	std::vector<int> digits(str.size());
	std::ranges::transform(str, digits.begin(), [](char c) { return c - '0'; });
	return digits;
}

}  // namespace oa::utils::str_utils
