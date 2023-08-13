#ifndef OPENANALYTICS_TIME_TENOR_H
#define OPENANALYTICS_TIME_TENOR_H

#include <string>

#include "oa/dllexport.h"
#include "time_enums.h"

//tc = tenor class
namespace oa::time
{
	class OA_TIME_API Tenor
	{
		public:
			Tenor(const std::string& input_string);
			/// <summary>
			/// flips the sign of the tenor
			/// </summary>
			/// <returns>New Tenor object withteh sign flipped</returns>
			const Tenor FlipSign() const;

			/// <summary>
			/// returns a pair that is the integer and the unit of time a char
			/// </summary>
			/// <returns>a pair where the first length and the second argument is lenght as a Enum</returns>
			const std::pair<int, oa::time::Tenors> GetValues() const;

		private:
			int m_number_ = 0;
			oa::time::Tenors m_time_unit_{};
	};
}



#endif // !OPENANALYTICS_TIME_TENOR_H
