#include <unordered_map>
#include <memory>
#include <string>
#include <algorithm>

#include "time/date.h"
#include "time/calendar.h"
#include "dao/calendar_dao.h"

#include <boost/algorithm/string.hpp>

#ifndef ODINANALYTICS_STATIC_DATA_CACHE_CALENDAR_CACHE_H_
#define ODINANALYTICS_STATIC_DATA_CACHE_CALENDAR_CACHE_H_



namespace oa::static_cache
{
	
	class CalendarCache
	{
		public:
			/// <summary>
			/// removing the copy constructor
			/// </summary>
			/// <param name=""></param>
			CalendarCache(const CalendarCache&) = delete;

			/// <summary>
			/// delete the assignment operator 
			/// </summary>
			/// <param name=""></param>
			/// <returns></returns>
			void operator=(const CalendarCache&) = delete;

			/// <summary>
			/// Returns a shared ptr of the Calendar that was 
			/// </summary>
			/// <param name="calendar_str"></param>
			/// <returns></returns>
			std::shared_ptr<oa::time::Calendar> GetCalendar(const std::string& calendars_str);

			/// <summary>
			/// retrieves the singleton instance of the cache 
			/// </summary>
			/// <param name=""></param>
			/// <returns></returns>
			static CalendarCache& RetrieveCache(void);

		private:
			std::unordered_map<std::string, std::shared_ptr<time::Calendar>> calendar_cache{};
			CalendarCache() = default;
			
			/// <summary>
			/// checks to see if a calendar is cached
			/// </summary>
			/// <param name="calendar_str"></param>
			/// <returns></returns>
			bool IsCached(const std::string& calendar_str);
			
			/// <summary>
			/// stores the calendar within the static map and saves it
			/// </summary>
			/// <param name="calendar_str">sorted conjoined string that is a unique key</param>
			/// <param name="calendar_data">a </param>
			void StoreCalendar(const std::string& calendar_str, const std::shared_ptr<time::Calendar> calendar_data);

	};
}

#endif // !ODINANALYTICS_STATIC_DATA_CACHE_CALENDAR_CACHE_H_
