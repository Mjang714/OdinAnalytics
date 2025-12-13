#ifndef ODINANALYTICS_DAO_CALENDAR_DAO_H_
#define ODINANALYTICS_DAO_CALENDAR_DAO_H_

#include <string>

#include "oa/dllexport.h"
// TODO: fix header namespacing
#include "time/calendar_data_struct.h"

namespace oa::dao
{

	class OA_DAO_API CalendarDao
	{


		public:
			/// <summary>
			/// remove the copy constrcutor
			/// </summary>
			/// <param name="obj"></param>
			CalendarDao(const CalendarDao& ) = delete;

			/// <summary>
			/// overload the equal operator
			/// </summary>
			/// <param name="obj"></param>
			/// <returns></returns>
			CalendarDao& operator=(const CalendarDao& ) = delete;

			/// <summary>
			/// retrieve the static instance of the Calendar Dao that can be used
			/// this design pattern follows Meyer's singleton
			/// this provides us with 2 features
			/// 1. thread safe intialization
			/// 2. only one instance exist at any given time and it will be destroyed
			/// </summary>
			/// <returns></returns>
			static CalendarDao& GetInstance();

			/// <summary>
			/// returns a data struct from flat files to get the calendar data from one at a time
			/// </summary>
			/// <param name="calendar_str"> what city calendar that is needed (i.e. NYB = New York Fed)</param>
			/// <returns></returns>
			static oa::ds::CalendarDataStruct GetCalendartData(const std::string& region);

		private:

			//for static member variables these must be inlined
			//static inline std::string calendar_file_path_str;
			static inline const std::string weekend_delimiter{ "#Weekend" };
			static inline const std::string holidays_delimiter{ "# Holidays" };

			CalendarDao() = default;
	};

}

#endif // !ODINANALYTICS_DAO_CALENDAR_DAO_H_
