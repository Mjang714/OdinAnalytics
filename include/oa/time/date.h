/**
 * @file time/date.h
 * @author Michael Jang, Derek Huang
 * @brief C++ header for a date class
 * @copyright MIT License
 */

#ifndef OA_TIME_DATE_H_
#define OA_TIME_DATE_H_

#include <chrono>
#include <compare>
#include <string>
#include <string_view>
#include <tuple>

#include "oa/dllexport.h"
#include "oa/time/enums.h"
#include "oa/time/tenor.h"

namespace oa::time {

/**
 * OA date class.
 *
 * This encapsulates a single Julian date integer and provides operations to
 * manipulate the date and perform useful conversions.
 *
 * @note Although in theory this class could be `constexpr` this would require
 *  that several of the implementation functions be made inline.
 */
class OA_TIME_API Date {
public:
	// provides compatibility with the system clock + C time structs
	using time_point = std::chrono::system_clock::time_point;
	// year, month, day tuple for Gregorian date components
	// note: may want to rename for better std::chrono::year_month_day interop
	using year_month_day = std::tuple<int, int, int>;

	/**
	 * Default ctor.
	 *
	 * This constructs a `Date` with a zero Julian day number that represents
	 * November 24, -4714, in proleptic Gregorian calendar.
	 *
	 * @todo If we want to make this represent an invalid date then we should
	 *  initialize the Julian day number to something negative.
	 */
	constexpr Date() noexcept = default;

	/**
	 * Ctor.
	 *
	 * Constructs a `Date` from the given Gregorian year, month, and day.
	 *
	 * @param year Gregorian year
	 * @param month Month number starting from 1
	 * @param day Day number of a month starting from 1
	 */
	Date(int year, unsigned month, unsigned day);

	/**
	 * Ctor.
	 *
	 * Construct a `Date` object from a Gregorian `YYYY-MM-DD` string, where
	 * the `-` delimiter may be instead replaced with `:` or `/`. If the given
	 * Gregorian date is invalid, an exception is thrown.
	 *
	 * @note This is slower than direct construction using a Julian day number
	 *  because of the need for string parsing but is more user-friendly.
	 *
	 * @param ymd [Proleptic] Gregorian year, month, and day string
	 */
	explicit Date(std::string_view ymd);

	/**
	 * Ctor.
	 *
	 * Valid Julian day numbers must be nonnegative.
	 *
	 * @param jdn Julian day number
	 */
	explicit constexpr Date(int jdn) noexcept : julian_{jdn} {}

	/**
	 * Ctor.
	 *
	 * Construct a `Date` from a `std::chrono` system time point.
	 *
	 * @param tp `std::chrono` system clock time point
	 */
	explicit Date(const time_point& tp);

	/**
	 * Converts a valid [proleptic] Gregorian date to a Julian day number.
	 *
	 * The result is good for valid [proleptic] Gregorian dates after November
	 * 23, -4713 which is more than sufficient for financial applications.
	 *
	 * @param year [Proleptic] Gregorian year
	 * @param month Month number starting from 1
	 * @param day Day number starting from 1
	 */
	static int ConvertToJulian(int year, int month, int day) noexcept;

	/// <summary>
	///  this function takes the date class internal parameters and creates a string form of the date
	/// </summary>
	/// <returns>a string of the date formatted "YYYY-MM-DD : Julian Integer = ######"</returns>
	std::string ToString() const;

	/// <summary>
	/// return wether or not a year is IsLeapYear
	/// </summary>
	/// <returns>boolean value if a year is leap</returns>
	bool IsLeap() const noexcept;

	/// <summary>
	///  return wether or not a year is IsLeapYear
	/// </summary>
	/// <param name="year">given a year</param>
	/// <returns>boolean value if a year is leap</returns>
	static bool IsLeap(int year) noexcept;

	// TODO: deprecate in favor of julian()
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
	time_point ConvertToTimePt() const;

	static time_point ConvertToTimePt(const Date& date);

	/**
	 * Returns the number of days within a month in the given Gregorian year.
	 *
	 * Proleptic Gregorian years are allowed and an exception will be thrown if
	 * the month value is invalid, i.e. not in `[1, 12]` inclusive.
	 *
	 * @param year [Proleptic] Gregorian year
	 * @param month Month number starting from 1
	 */
	static unsigned DaysInMonth(int year, unsigned month);

	// TODO: implemented using operator+
	/// <summary>
	/// takes in a tenor and adds it the date objects
	/// </summary>
	/// <param name="tenor">given a year</param>
	/// <returns>a new date that has the tenor added</returns>
	Date AddTenor(const oa::time::Tenor& tenor) const;

	// TODO: implemented using operator-
	/// <summary>
	///  takes in a tenor and subtracts it from the date object
	/// </summary>
	/// <param name="tenor"></param>
	/// <returns>a new date that has the tenor subtracted</returns>
	Date SubTenor(const oa::time::Tenor& tenor) const;

	/**
	 * Return the Julian day number.
	 */
	int julian() const noexcept;

	/**
	 * Return the [proleptic] Gregorian year, month, and day.
	 */
	year_month_day gregorian() const;

	/**
	 * Returns the [proleptic] Gregorian year associated with the date.
	 *
	 * @note Use `gregorian()` to more efficiently retrieve all components.
	 */
	int year() const;

	/**
	 * Returns the [proleptic] Gregorian month associated with the date.
	 *
	 * @note Use `gregorian()` to more efficiently retrieve all components.
	 */
	int month() const;

	/**
	 * Returns the [proleptic] Gregorian day associated with the date.
	 *
	 * @note Use `gregorian()` to more efficiently retrieve all components.
	 */
	int day() const;

	/**
	 * Indicate if the `Date` is valid or not.
	 *
	 * The `Date` is invalid if the Julian date number is negative. To check if
	 * a `Date` was default constructed, compare against a defaulted instance.
	 *
	 * @note The semantics have changed since the original implementation which
	 *  simply checked to see if the `Date` was defaulted or not.
	 */
	explicit operator bool() const noexcept;

	/**
	 * Return self if valid and not defaulted otherwise the given date.
	 *
	 * @note Since a defaulted `Date` is technically valid it is a bit tricky
	 *  to use one as an "optional" value. We might want to encourage use of
	 *  `std::optional<Date>` later and remove this feature.
	 *
	 * @param other Default date value to use
	 */
	Date value_or(Date other) const noexcept;

	/**
	 * Defaulted three-way comparison operator.
	 *
	 * The only member to compare is the Julian day number which provides a
	 * convenient total ordering of all the dates.
	 *
	 * @note Only non-member declarations can take the target object by value.
	 */
	constexpr auto operator<=>(const Date&) const noexcept = default;

	/**
	 * Add a given `Tenor` to the date.
	 *
	 * @param tenor Tenor to add to the date
	 */
	Date operator+(const Tenor& tenor) const;

	/**
	 * Subtract a given `Tenor` from the date.
	 *
	 * @param tenor Tenor to subtract from the date
	 */
	Date operator-(const Tenor& tenor) const;

private:
	// Julian day number
	// note: only member of the class to ensure it fits in a register
	int julian_{};

	static constexpr int kChronoYearOffset = 1900;
	static constexpr int kMonthOffset = 1;

	// helper functions to produce new Dates given different time adjustments
	Date AddDays(int days) const noexcept;
	Date AddWeeks(int weeks) const noexcept;
	Date AddMonths(int months) const;
	Date AddYears(int years) const;

	/**
	 * Convert a valid Julian day number into a [proleptic] Gregorian date.
	 *
	 * This uses the Richards algorithm described in the Julian day Wikipedia
	 * page here: https://en.wikipedia.org/wiki/Julian_day
	 *
	 * @param jdn Julian day number
	 */
	static year_month_day YMD(const int jdn) noexcept;

	/**
	 * Convert a `std::chrono` system clock time point into a Gregorian date.
	 *
	 * @param tp `std::chrono` system clock time point
	 */
	static year_month_day YMD(const time_point& tp);
};

/**
 * Add the given number of days to the date.
 *
 * @param date Date to adjust
 * @param days Number of days to add/subtract
 */
OA_TIME_API
Date operator+(const Date& date, int days) /*noexcept*/;

/**
 * Add the given number of days to the date.
 *
 * @param days Number of days to add/subtract
 * @param date Date to adjust
 */
OA_TIME_API
Date operator+(int days, const Date& date) /*noexcept*/;

/**
 * Add a given `Tenor` to the date.
 *
 * @param tenor Tenor to add to the date
 * @param date Date to adjust
 */
OA_TIME_API
Date operator+(const Tenor& tenor, const Date& date);

/**
 * Subtract the given number of days from the date.
 *
 * @note This operator is not symmetric like `operator+` as you cannot
 *  subtract a date from an integer number of days.
 *
 * @param date Date to adjust
 * @param days Number of days to add/subtract
 */
OA_TIME_API
Date operator-(const Date& date, int days) /*noexcept*/;

}  // namespace oa::time

#endif  // OA_TIME_DATE_H_
