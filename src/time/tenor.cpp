#include "tenor.h"

#include <boost/algorithm/string.hpp>

#include "helpers/utils.h"

namespace oa
{
	namespace time
	{
		Tenor::Tenor(const std::string& input_string)
		{
			try
			{
				if (oa::utils::CheckTenorStr(input_string))
				{
					m_number_ = std::stoi(input_string.substr(0, input_string.size() - 1));
					m_time_unit_ = oa::utils::ValueToEnum<time::Tenors>(toupper(input_string.back()));
				}
				else
				{
					throw "Invalid tenor type please check your input of " + input_string;
				}
			}
			catch (const std::exception&) {}
		}

		const Tenor Tenor::FlipSign() const
		{
			return Tenor(std::to_string( -m_number_) +
				static_cast<char> (m_time_unit_));
		}

		const std::pair<int, oa::time::Tenors>Tenor::GetValues() const
		{
			return std::pair<int, oa::time::Tenors>(m_number_, m_time_unit_);
		}
	}
}