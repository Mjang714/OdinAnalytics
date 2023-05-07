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

	std::shared_ptr<CalendarTypeAlias> CalendarCache::GetCalendar(const std::string& calendar_str)
	{
		if (IsCached(calendar_str))
		{
			return calendar_cache.at(calendar_str);
		}

		//if we can't find the calendar store it 
		else 
		{
			oa::ds::CalendarDataStruct raw_calendar_data = 
				DaoObj::GetInstance().GetCalendartData(calendar_str);

			auto new_calendar = 
				std::make_shared<CalendarTypeAlias>(raw_calendar_data);

			StoreCalendar(calendar_str, new_calendar);
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