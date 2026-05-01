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


		Tenor::Tenor(int time_length, oa::time::Tenors tenor_enum) : m_number_(time_length), m_time_unit_(tenor_enum)
		{

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

		//that fact I need this function is really sign I messed up somewhere. Maybe we ca instead implement a hash function  using the string of the tenor?
		bool Tenor::operator==(const Tenor& other) const
		{
			return this->m_number_ == other.m_number_ && this->m_time_unit_ == other.m_time_unit_;
		}
	}
}