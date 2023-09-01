#ifndef OPENANALYTICS_TIME_DATES_H
#define OPENANALYTICS_TIME_DATES_H

#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <compare>
#include <string>
#include <tuple>

#include "helpers/utils.h"
#include "oa/dllexport.h"
#include "tenor.h"
#include "time_enums.h"

//OA = Odin Analytics Date Class

namespace oa::time
{

	class OA_TIME_API Date
	{
	public:
		//constrcutors
		Date() = default;

		//we will make this constructor private to prevent someone from trying to create a whacky date that makes no sense
		explicit Date(const int& year, const int& month, const int& day);

		/// <summary>
		/// This function will be the main interface for the date class and takes a date as a stringd
		/// </summary>
		/// <param name="date_string">
		/// string representation of the date that
		/// will be checked for correctness and fed into the private construtor Date(const long long julian_date)
		/// </param>
		explicit Date(const std::string& date_str);

		/// <summary>
		/// constructor will take in julian date number and convert it back into it constitute YEAR MONTH and DATE
		/// by calling ConvertToGreg
		/// </summary>
		/// <param name="julian_date"></param>
		explicit Date(const int& julian_int);

		/// <summary>
		/// takes a time point and returns a time Date representation
		/// </summary>
		/// <param name="time_point">takes in time_point object</param>
		explicit Date(const std::chrono::system_clock::time_point& time_point);

		/// <summary>
		/// the algorithm to convert gregorian Calendar date to Julian date is from this
		/// </summary>
		/// <param name="year"></param>
		/// <param name="month"></param>
		/// <param name="day"></param>
		/// <returns>a long int of the julian date number</returns>
		static int ConvertToJulian(int year,int month,int day);

		/// <summary>
		///  this function takes the date class internal parameters and creates a string form of the date
		/// </summary>
		/// <returns>a string of the date formatted "YYYY-MM-DD : Julian Integer = ######"</returns>
		std::string ToString() const;

		/// <summary>
		/// return wether or not a year is IsLeapYear
		/// </summary>
		/// <returns>boolean value if a year is leap</returns>
		bool IsLeap(void) const;

		/// <summary>
		///  return wether or not a year is IsLeapYear
		/// </summary>
		/// <param name="year">given a year</param>
		/// <returns>boolean value if a year is leap</returns>
		static bool IsLeap(int year);

		/// <summary>
		///  Functions that returns the julian date integer of the the date
		/// </summary>
		/// <returns>int</returns>
		int GetJulian() const;

		/// <summary>
		///  return an integer that represents what day of the week it is (0 for Monday - 6 for Sunday)
		/// </summary>
		/// <returns>integer representation of what day of the week it is</returns>
		int GetDOWInt() const;

		/// <summary>
		/// static function that returst the DOW as an integer
		/// </summary>
		/// <param name="julian_date"></param>
		/// <returns></returns>
		static int GetDOWInt(int julian_date);


		/// <summary>
		/// this is the julian date offest to take in excel double date
		/// and convert it to a proper julian
		/// </summary>
		static constexpr int kXlJulianOffSet {2415019};


		/// <summary>
		/// converts a date object to time point object
		/// https://stackoverflow.com/questions/66537237/how-to-get-chrono-time-point-from-year-month-day-hour-minute-second-millis
		/// </summary>
		/// <param name="given_date"></param>
		/// <returns>time point object where seconds are defaulted to zero</returns>
		std::chrono::system_clock::time_point ConvertToTimePt(void) const;

		static std::chrono::system_clock::time_point ConvertToTimePt(const Date& given_date);

		/// <summary>
		/// this is function that is public that will take a month enum
		/// and get the number of the days in the month
		/// </summary>
		/// <returns>the number of days within a month as an integer </returns>
		static int DaysInMonth(int month, int year);

		/// <summary>
		/// takes in a tenor and adds it the date objects
		/// </summary>
		/// <param name="tenor">given a year</param>
		/// <returns>a new date that has the tenor added</returns>
		Date AddTenor(const oa::time::Tenor& tenor) const;

		/// <summary>
		///  takes in a tenor and subtracts it from the date object
		/// </summary>
		/// <param name="tenor"></param>
		/// <returns>a new date that has the tenor subtracted</returns>
		Date SubTenor(const oa::time::Tenor& tenor) const;

		/// <summary>
		/// methods to return days, months, and year
		/// </summary>
		/// <returns>int value</returns>
		int m_years() const { return m_years_; };
		int m_months() const { return m_months_; };
		int m_days() const { return m_days_; };

		//operator overloads leveraging c++ 20 sapceship operator
		// Since we are basing comparisons on julian dates we will most likely be using strong ordering
		bool operator==(const Date& right_value) const;
		std::strong_ordering operator<=>(const Date& right_value) const;



	private:
		int m_days_ = 0, m_months_= 0, m_years_ = 0, m_julian_int_ = 0;

		const static int kChronoYearOffset = 1900;
		const static int kMonthOffset = 1;

		Date AddDays(const int& time_length) const;

		Date AddWeeks(const int& time_length) const;

		Date AddMonths(const int& time_length) const;

		Date AddYears(const int& time_length) const;

		Date SubDays(const int& time_length) const;

		Date SubWeeks(const int& time_length) const;

		Date SubMonths(const int& time_length) const;

		Date SubYears(const int& time_length) const;

		/// <summary>
		/// converts the julian day to its year month and day and returns it as a tuple
		/// using the algorithm desscribed in https://en.wikipedia.org/wiki/Julian_day
		/// </summary>
		/// <param name="julian_day">long long int</param>
		/// <returns>a tuple where the first element is the year then month and day</returns>
		static std::tuple<int, int, int> ConvertToGregInt(const int);

		/// <summary>
		/// takes a time point obejct and converts it to year month and date using
		/// https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c
		/// </summary>
		/// <param name="time_point">input into the </param>
		/// <returns>a tuple where the first element is the year then month and day</returns>
		static std::tuple<int, int, int> ConvertToGregInt(
			const std::chrono::system_clock::time_point& time_point);

		/// <summary>
		/// takes in a tuple and populates year month and date
		/// </summary>
		/// <param name="result"></param>
		void PopulateYMDFromTuple(const std::tuple<int, int, int>& results);

	};

}  // namespace oa::time

#endif  // OPENANALYTICS_TIME_DATES_H