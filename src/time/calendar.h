#ifndef OPENANALYTICS_TIME_CALENDAR_H
#define OPENANALYTICS_TIME_CALENDAR_H

#include<bitset>
#include<string>
#include<unordered_set>
#include<vector>

#include "calendar_data_struct.h"
#include "date.h"

namespace oa::time
{
		
	class Calendar
	{
		public:
			Calendar() = default;
			explicit Calendar(const std::vector<ds::CalendarDataStruct>& calendar_data);
			explicit Calendar(const ds::CalendarDataStruct& calendar_data);
				
			/// <summary>
			/// Checks to see if a given date is a holiday 
			/// </summary>
			/// <param name="given_date"></param>
			/// <returns>boolean value</returns>
			bool IsHoliday(const oa::time::Date& given_date) const;

			/// <summary>
			/// This is function to take in julian date repsenation of a date 
			/// </summary>
			/// <param name="julian_date"></param>
			/// <returns>boolean whether or not a given day is business day</returns>
			bool IsHoliday( int julian_date) const;

			/// <summary>
			/// Checks to see if a given date is a Biz Day 
			/// </summary>
			/// <param name="given_date"></param>
			/// <returns>boolean whether or not a given day is business day</returns>
			bool IsBusinessDay(const oa::time::Date& given_date) const;

			/// <summary>
			/// Checks to see if a given date is a Biz Day 
			/// </summary>
			/// <param name="given_date"></param>
			/// <returns>boolean whether or not a given day is business day</returns>
			bool IsBusinessDay(const int given_date) const;

			/// <summary>
			/// takes a date and returns the next business date
			/// </summary>
			/// <param name="base_date">Date object</param>
			/// <returns>date object that is the next business day</returns>
			oa::time::Date GetNextBusinessDay(const oa::time::Date& base_date) const;

			/// <summary>
			/// takes a date and returns the previosu business day
			/// </summary>
			/// <param name="base_date"></param>
			/// <returns>a date object that is the previous businessday</returns>
			oa::time::Date GetPrevBusinessDay(const oa::time::Date& base_date) const;

		private:
			/// <summary>
			/// populate the regions and the related data fields 
			/// </summary>
			/// <param name="holidays"></param>
			void PopulateHolidays(const std::vector<ds::CalendarDataStruct>& calendar_data);

			/// <summary>
			///  will take an integer that is passed by value No need to make this immutable
			/// </summary>
			/// <param name="julian_base_date"></param>
			/// <returns>Date object</returns>
			inline oa::time::Date GetNextBusinessDay(int julian_base_date) const;

			/// <summary>
			/// returns a date object that is the previous date
			/// </summary>
			/// <param name="julian_base_date"></param>
			/// <returns>Date object</returns>
			oa::time::Date GetPrevBusinessDay(int julian_base_date) const;

			std::bitset<7> m_weekends_;
			std::string m_regions_ = "";
			std::unordered_set<int> m_holidays_ {};
	};
	
}


#endif //OPENANALYTICS_TIME_CALENDAR_H


