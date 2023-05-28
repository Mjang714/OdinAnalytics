#include "calendar_cache.h"

namespace oa::static_cache
{
	typedef  oa::time::Calendar CalendarTypeAlias;
	typedef oa::dao::CalendarDao DaoObj;

	CalendarCache& CalendarCache::RetrieveCache(void)
	{
		static CalendarCache calendar_cache{};
		return calendar_cache;
	}

	std::shared_ptr<CalendarTypeAlias> CalendarCache::GetCalendar(const std::string& calendars_str)
	{
		std::vector<std::string> list_of_caledars{};
		boost::split(list_of_caledars, calendars_str, boost::is_any_of(",:;."));
		std::sort(list_of_caledars.begin(), list_of_caledars.end());
		std::string str_key = boost::algorithm::join(list_of_caledars, ",");
		
		if (IsCached(str_key))
		{
			return calendar_cache.at(str_key);
		}

		//if we can't find the calendar store it 
		else 
		{
			std::vector<oa::ds::CalendarDataStruct> raw_calendar_data;
			for (const auto& calendar_file_name : list_of_caledars) 
			{
				raw_calendar_data.push_back(
					DaoObj::GetInstance().GetCalendartData(calendar_file_name));
			}
			

			auto new_calendar = 
				std::make_shared<CalendarTypeAlias>(raw_calendar_data);

			StoreCalendar(str_key, new_calendar);
			return new_calendar;

		}
	}

	bool CalendarCache::IsCached(const std::string& calendar_str)
	{
		if (calendar_cache.contains(calendar_str))
		{
			return true;
		}
		return false;
	}

	void CalendarCache::StoreCalendar(const std::string& calendar_str, const std::shared_ptr<time::Calendar> calendar_data)
	{
		calendar_cache.insert({ calendar_str, calendar_data });
	}
}