/**
 * @file time/date.cpp
 * @author Michael Jang, Derek Huang
 * @brief C++ source for a date class
 * @copyright MIT License
 */

#include "oa/time/date.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include "helpers/utils.h"
#include "oa/string.h"
#include "oa/to.h"
#include "oa/time/tenor.h"
#include "oa/time/enums.h"

namespace oa::time {

Date::Date(int year, int month, int day)
{
	// check that number of days is valid
	// note: DaysInMonth() will throw if the month value is invalid
	if (!day || day > DaysInMonth(year, month))
		throw std::runtime_error{"invalid day number " + std::to_string(day)};
	// all good, so convert to Julian day
	julian_ = ConvertToJulian(year, month, day);
}

Date::Date(std::string_view ymd)
{
	// convert to Gregorian date (throws if string is invalid) + convert
	auto [y, m, d] = YMD(ymd);
	julian_ = ConvertToJulian(y, m, d);
}

Date::Date(const time_point& tp)
{
	auto [y, m, d] = YMD(tp);
	julian_ = ConvertToJulian(y, m, d);
}

Date::year_month_day
Date::YMD(const time_point& tp)
{
	// convert to std::time_t integral time
	auto c_time = std::chrono::system_clock::to_time_t(tp);
	// get pointer to std::tm in GMT (UTC) and return time tuple
	auto c_tm = std::gmtime(&c_time);
	return {1900 + c_tm->tm_year, 1 + c_tm->tm_mon, c_tm->tm_mday};
}

Date::year_month_day
Date::YMD(std::string_view ymd)
{
	// check if date is one of the valid delimiters
	auto is_delim = [](char c) noexcept
	{
		switch (c) {
		case '/':
		case '-':
		case ':':
			return true;
		default:
			return false;
		}
	};
	// parse the date string components
	auto n_begin = ymd.begin();
	auto n_end = ymd.begin();
	// parse year
	while (n_end != ymd.end() && is_digit(*n_end))
		n_end++;
	// error if range is empty, wrong size, no month, or delimiter is invalid
	// note: n_begin == n_end also catches case of empty date string
	// note: always do n_end - n_begin as MSVC STL is strict and in debug mode
	// raises an assertion if you try to seek past the end of a string_view
	if (n_begin == n_end)
		throw std::runtime_error{"date missing year component"};
	if (n_end - n_begin != 4)
		throw std::runtime_error{"date year field must be 4 characters wide"};
	if (n_end == ymd.end())
		throw std::runtime_error{"date string truncated (no month)"};
	if (!is_delim(*n_end))
		throw std::runtime_error{"encountered invalid year delimiter"};
	// save as current date delimiter + convert year
	auto delim = *n_end;
	auto y = to<int>(n_begin, n_end, unsafe);
	// parse month
	n_begin = ++n_end;
	while (n_end != ymd.end() && is_digit(*n_end))
		n_end++;
	// error if range is empty, wrong size, no day, or delimiter is invalid
	if (n_begin == n_end)
		throw std::runtime_error{"date missing month component"};
	if (n_end - n_begin > 2)
		throw std::runtime_error{"date month field must be <=2 characters wide"};
	if (n_end == ymd.end())
		throw std::runtime_error{"date string truncated (no day)"};
	if (*n_end != delim)
		throw std::runtime_error{"encountered invalid/inconsistent month delimiter"};
	// convert month
	auto m = to<int>(n_begin, n_end, unsafe);
	// parse day
	n_begin = ++n_end;
	while (n_end != ymd.end() && is_digit(*n_end))
		n_end++;
	// error if range is empty, wrong size, or didn't reach end of string
	if (n_begin == n_end)
		throw std::runtime_error{"date missing day component"};
	if (n_end - n_begin > 2)
		throw std::runtime_error{"date month field must be <=2 characters wide"};
	if (n_end != ymd.end())
		throw std::runtime_error{"date string contains extra characters"};
	// convert day
	auto d = to<int>(n_begin, n_end, unsafe);
	// check that number of days is valid
	// note: DaysInMonth() will throw if the month value is invalid
	if (!d || d > DaysInMonth(y, m))
		throw std::runtime_error{"invalid day number " + std::to_string(d)};
	// done
	return {y, m, d};
}

Date::year_month_day
Date::YMD(const int julian_day) noexcept
{
	int y = 4716, j = 1401, m = 2,   n = 12, r = 4,      p = 1461,
		v = 3,    u = 5,    s = 153, w = 2,  B = 274277, C = -38;

	int f = julian_day + j + (((4 * julian_day + B) / 146097) * 3) / 4 + C;
	int e = r * f + v;
	int g = (e % p) / r;
	int h = u * g + w;

	int computed_days = (h % s) / u + 1;
	int computed_months = (((h / s) + m) % n) + 1;
	int computed_years = (e / p) - y + (n + m - computed_months) / n;

	return {computed_years, computed_months, computed_days};
}

int
Date::ConvertToJulian(int year, int month, int day) noexcept
{
	return (1461 *(year + 4800 + (month - 14) / 12)) / 4 +
		(367 *(month - 2 - 12 *((month - 14) / 12))) / 12 -
		(3 *((year + 4900 + (month - 14) / 12) / 100)) / 4
		+ day - 32075;
}

int
Date::GetJulian() const
{
	return julian_;
}

int Date::GetDOWInt() const
{
	return GetDOWInt(julian_);
}

int Date::GetDOWInt(int julian_date)
{
	return julian_date % 7;
}

Date::time_point
Date::ConvertToTimePt() const
{
	return ConvertToTimePt(*this);
}

Date::time_point
Date::ConvertToTimePt(const Date& date)
{
	std::tm time_point_result {};

	auto [y, m, d] = YMD(date.julian());

	time_point_result.tm_year = y - 1900;
	time_point_result.tm_mon = m - 1;
	time_point_result.tm_mday = d;
	time_point_result.tm_hour = 0;
	time_point_result.tm_min = 0;
	time_point_result.tm_sec = 0;
	time_point_result.tm_isdst = -1;

	return std::chrono::system_clock::from_time_t(std::mktime(&time_point_result));
}

std::string
Date::ToString() const
{
	std::stringstream ss;
	auto [y, m, d] = YMD(julian_);
	// note: this format is a bit different from operator<<
	ss << y << '-' << m << '-' << d << " : Julian Integer = " << julian_;
	// note: C++20 allows move-constructing using ref-qualified str()
	return std::move(ss).str();
}

bool
Date::IsLeap() const noexcept
{
	// note: silence warnings about m, d being unused
	[[maybe_unused]] auto [y, m, d] = YMD(julian_);
	return IsLeap(y);
}

bool
Date::IsLeap(int year) noexcept
{
	return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

int
Date::DaysInMonth(int year, int month)
{
	// sanity check
	if (month < 1 || month > 12)
		throw std::runtime_error("month must be in the range [1, 12]");
	// simple switch
	switch (month) {
	case 2:
		return (IsLeap(year)) ? 29 : 28;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	default:
		return 31;
	}
}

Date Date::SubTenor(const Tenor& tenor) const
{
	return *this - tenor;
}

Date Date::AddTenor(const Tenor& tenor) const
{
	return *this + tenor;
}

Date
Date::AddDays(int days) const noexcept
{
	return Date(julian_ + days);
}

Date
Date::AddWeeks(int weeks) const noexcept
{
	return Date(julian_ + 7 * weeks);
}

Date
Date::AddMonths(int months) const
{
	// get Gregorian year, month, day
	auto [year, month, day] = YMD(julian_);
	// compute the numbers of years + months
	int new_year = 0;
	int new_month = 0;
	if (months > 0)
	{
		new_year = year + (month + months) / 12;
		new_month = (month + months) % 12;
		// added this for when you are in June, add 6M, then land in December,
		// as the year count ends up being incremented
		if (new_month <= 0)
		{
			new_month = new_month + 12;
			new_year--;
		}
	}
	else
	{
		new_year = year + (month + months) / 12;
		new_month = (month + months) % 12;
		if (new_month < 1)
		{
			new_year += -1;
		}

		//check for when we add negative dates
		if (new_month <= 0)
		{
			new_month = new_month + 12;
		}
	}
	// logic to check the day part is right
	auto new_day = day;
	// note: DaysInMonth() should not throw here
	if (new_day > DaysInMonth(new_year, new_month))
	{
		new_day = DaysInMonth(new_year, new_month);
	}
	// create from new Gregorian year, month, day
	return {new_year, new_month, new_day};
}

Date
Date::AddYears(int years) const
{
	// get Gregorian year, month, day + add years
	auto [year, month, day] = YMD(julian_);
	year += years;
	//if it is not leap year then check to see if this
	// TODO: not sure what the above comment means. but looks like a special
	// check to handle February when we aren't in a leap year
	if (!IsLeap(year) && (month == 2))
	{
		auto month_days = DaysInMonth(year, month);
		day = (day > month_days) ? month_days : day;
	}
	return {year, month, day};
}

int
Date::julian() const noexcept
{
	return julian_;
}

Date::year_month_day
Date::gregorian() const
{
	return YMD(julian_);
}

int
Date::year() const
{
	return std::get<0>(gregorian());
}

int
Date::month() const
{
	return std::get<1>(gregorian());
}

int
Date::day() const
{
	return std::get<2>(gregorian());
}

Date::operator bool() const noexcept
{
	return !!julian_;
}

Date
Date::value_or(Date other) const noexcept
{
	return (julian_) ? *this : other;
}

Date
Date::operator+(const Tenor& tenor) const
{
	switch (tenor.unit()) {
	case Tenors::kDays:
		return AddDays(tenor.count());
	case Tenors::kWeeks:
		return AddWeeks(tenor.count());
	case Tenors::kMonths:
		return AddMonths(tenor.count());
	case Tenors::kYears:
		return AddYears(tenor.count());
	default:
		throw std::runtime_error{
			"Not a valid Tenor please check input tenor of:" +
			utils::GetCleanName(tenor.unit())
		};
	}
}

Date
Date::operator-(const Tenor& tenor) const
{
	return *this + -tenor;
}

////////////////////////////////////////////////////////////////////////////////
// operators                                                                  //
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const Date& date)
{
	// save current stream fill character and set to '0'
	// note: if an exception were thrown in this function the fill character
	// would not be reset. for exception safety a scope guard is necessary
	auto fill_char = out.fill();
	out.fill('0');
	// get Gregorian date components
	auto [y, m, d] = date.gregorian();
	// print in YYYY-MM-DD
	// note: std::setw resets each after operator<< on an integral value
	out <<
		std::setw(4) << y << "-" <<
		std::setw(2) << m << "-" <<
		std::setw(2) << d;
	// fill character needs to be manually reset
	out.fill(fill_char);
	return out;
}

Date operator+(const Date& date, int days) /*noexcept*/
{
	return Date{date.julian() + days};
}

Date operator+(int days, const Date& date) /*noexcept*/
{
	return date + days;
}

Date operator+(const Tenor& tenor, const Date& date)
{
	return date + tenor;
}

Date operator-(const Date& date, int days) /*noexcept*/
{
	return Date{date.julian() - days};
}

}  // namespace oa::time
