#include "calendar.h"

#include "helpers/utils.h"
#include "helpers/warnings.h"

namespace oa
{
	namespace time
	{

		Calendar::Calendar(const std::vector<ds::CalendarDataStruct>& calendar_data)
		{
			PopulateHolidays(calendar_data);

		}

		Calendar::Calendar(const ds::CalendarDataStruct& calendar_data)
		{
			PopulateHolidays(std::vector<ds::CalendarDataStruct>{calendar_data});
		}

		void Calendar::PopulateHolidays(const std::vector<ds::CalendarDataStruct>& calendar_data)
		{
			for (const auto& calendar : calendar_data)
			{
				if (m_regions_ == "")
				{
					m_regions_ += calendar.regions;
				}

				else
				{
					m_regions_ += ("," + calendar.regions);

				}

				//populate the holidays
				if (calendar.list_of_holidays.size() != 0)
				{
					for (auto given_holiday : calendar.list_of_holidays)
					{
						m_holidays_.insert(given_holiday.GetJulian());
					}
				}

				//populate the weekends
				if (calendar.weekends.size() != 0)
				{
					for (auto given_weekend : calendar.weekends)
					{
// suppress signed/unsigned mismatch that MSVC complains
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4365)
						m_weekends_[oa::utils::EnumToInt(given_weekend)] = 1;
OA_MSVC_WARNING_POP()
					}
				}
			}


		}


		bool Calendar::IsHoliday(const oa::time::Date& given_date) const
		{
			return IsHoliday(given_date.GetJulian());
		}

		bool Calendar::IsHoliday(int julian_date) const
		{
// suppress signed/unsigned mismatch that MSVC complains about
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4365)
			// check to see if the given date is a weekend
			if (m_weekends_[oa::time::Date::GetDOWInt(julian_date)])
				return true;
OA_MSVC_WARNING_POP()

			//check to see if it is in the
			else if (m_holidays_.contains(julian_date))
			{
				return true;
			}
			//if it is not return false cause it must be valid business day
			else
			{
				return false;
			}
		}

		bool Calendar::IsBusinessDay(const oa::time::Date& given_date) const
		{
			return !IsHoliday(given_date);
		}

		bool Calendar::IsBusinessDay(const int given_date) const
		{
			return !IsHoliday(given_date);
		}

		oa::time::Date Calendar::GetNextBusinessDay(const oa::time::Date& base_date) const
		{
			return GetNextBusinessDay(base_date.GetJulian());
		}

		inline oa::time::Date Calendar::GetNextBusinessDay(int julian_base_date) const
		{
			julian_base_date++;
			while (IsHoliday(julian_base_date))
			{
				julian_base_date++;
			}

			return oa::time::Date(julian_base_date);
		}

		oa::time::Date Calendar::GetPrevBusinessDay(const oa::time::Date& base_date) const
		{
			return GetPrevBusinessDay(base_date.GetJulian());
		}

		inline oa::time::Date Calendar::GetPrevBusinessDay(int julian_base_date) const
		{
			julian_base_date--;
			while (IsHoliday(julian_base_date))
			{
				julian_base_date--;
			}

			return oa::time::Date(julian_base_date);
		}
	}
}

