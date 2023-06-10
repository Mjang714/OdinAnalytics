#include<string>
#include<stdlib.h>
#include<vector>


#ifndef ODINANALYTICS_HELPERS_STR_UTILS_STR_UTILS_H_
#define ODINANALYTICS_HELPERS_STR_UTILS_STR_UTILS_H_

namespace oa::utils::str_utils
{
	/// <summary>
	/// this function takes in  a str and checks to see if every char is digit 
	/// right now it only works for positive digits but will add support for either
	/// </summary>
	/// <param name="str_to_parse"></param>
	/// <returns></returns>
	bool IsDigit(const std::string& str_to_parse) noexcept;

	/// <summary>
	/// takes a str of digits (i.e "091234" and returns vector of integers such as {0,9,1,2,3,4})
	/// </summary>
	/// <param name="str_to_parse"></param>
	/// <returns></returns>
	std::vector<int> StrToDigits(const std::string& str_to_parse) noexcept;

}


#endif // ODINANALYTICS_HELPERS_STR_UTILS_STR_UTILS_H_



