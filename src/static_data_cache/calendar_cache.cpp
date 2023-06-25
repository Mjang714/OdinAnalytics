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

	std::shared_ptr<const CalendarTypeAlias> CalendarCache::GetCalendar(const std::string& calendars_str)
	{
		std::vector<std::string> list_of_caledars{};
		boost::split(list_of_caledars, calendars_str, boost::is_any_of(",:;."));
		std::ranges::sort(list_of_caledars.begin(), list_of_caledars.end());
		std::string str_key = boost::algorithm::join(list_of_caledars, ",");
		
		//call scope mutex lock to lock down the cache member variable 
		std::scoped_lock<std::mutex> lock(m_cache_mutex_);

		if (IsCached(str_key))
		{
			return m_calendar_cache_.at(str_key);
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
				std::make_shared<const CalendarTypeAlias>(raw_calendar_data);

			StoreCalendar(str_key, new_calendar);
			return new_calendar;

		}
	}

	bool CalendarCache::IsCached(const std::string& calendar_str) const
	{
		if (m_calendar_cache_.contains(calendar_str))
		{
			return true;
		}
		return false;
	}

	void CalendarCache::StoreCalendar(const std::string& calendar_str, const std::shared_ptr<const time::Calendar> calendar_data)
	{
		
		//we dont need duplicates of the calendar just one so us try_emplace
		m_calendar_cache_.try_emplace( calendar_str, calendar_data );
	}
}