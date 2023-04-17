#include "oxl/xl_api/xl_converter_funcs.h"

namespace oxl::xl_api
{
	XlDictionary XlArrayToXlDictionary(const XlArray& xl_array)
	{
		size_t rows = xl_array.rows();
		size_t cols = xl_array.cols();

		if ((rows != 2) && (cols != 2))
		{
			throw std::invalid_argument("xl_converter_funcs.cpp line 12 XlArrayToXlDictionary(): Input was not correctly formatted please check to make sure is is 2xN or NX2");
		}

		XlDictionary xl_dictionary{};
		std::string key{};

		if (cols == 2)
		{
			//if it is Nx2 we will presume the first column is the key
			for (size_t i = 0; i < rows; i++)
			{
				key = std::get<std::string>(xl_array(i,0));
				xl_dictionary[key] = xl_array(i,1);
			}
		}

		else
		{
			//2xN we presume the first row is the key
			for (size_t j = 0; j < cols; j++)
			{
				key = std::get<std::string>(xl_array(0,j));
				xl_dictionary[key] = xl_array(1,j);
			}
		}

		return xl_dictionary;
	}

	XlArray XlDictionaryToXlArray(const XlDictionary& xl_dictionary)
	{
		std::vector<std::pair<std::string, XlVariant>> key_value_pair_list = xl_dictionary.GetKeyValuePair();
		XlArray xl_array(key_value_pair_list.size(), 2);
		size_t i = 0;
		for (const auto& key_value_pair : key_value_pair_list)
		{
			xl_array(i, 0) = key_value_pair.first;
			xl_array(i, 1) = key_value_pair.second;
			i++;
		}
		return xl_array;
	}
}