#include "date.h"

#include <chrono>
#include <ctime>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>

#include "helpers/utils.h"
#include "oa/platform.h"
#include "oa/time/tenor.h"
#include "oa/time/enums.h"

#if OA_HAS_CPP20_FORMAT
#include <format>
#endif  // !OA_HAS_CPP20_FORMAT

namespace oa::time {

// TODO: need sanity checks on the year, month, and day values
Date::Date(int year, unsigned month, unsigned day)
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
	// check if a character is an ASCII digit
	// TODO: add an equivalent function in oa/string.h
	auto is_digit = [](char c) noexcept
	{
		return (c >= '0') && (c <= '9');
	};
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
	// error if range is empty or delimiter is invalid
	// note: n_begin == n_end also catches case of empty date string
	if (n_begin == n_end)
		throw std::runtime_error{"date missing year component"};
	if (!is_delim(*n_end))
		throw std::runtime_error{"encountered invalid year delimiter"};
	// save as current date delimiter + convert year
	auto delim = *n_end;
	// TODO: kind of inefficient to create a new temporary string
	auto y = std::stoi(std::string{n_begin, n_end});
	// parse month
	n_begin = ++n_end;
	while (n_end != ymd.end() && is_digit(*n_end))
		n_end++;
	// error if range is empty or delimiter is invalid
	if (n_begin == n_end)
		throw std::runtime_error{"date missing month component"};
	if (*n_end != delim)
		throw std::runtime_error{"encountered invalid/inconsistent month delimiter"};
	// convert month
	// TODO: kind of inefficient to create a new temporary string
	auto m = std::stoi(std::string{n_begin, n_end});
	// parse day
	n_begin = ++n_end;
	while (n_end != ymd.end() && is_digit(*n_end))
		n_end++;
	// error if range is empty or didn't reach end of string
	if (n_begin == n_end)
		throw std::runtime_error{"date missing day component"};
	if (n_end != ymd.end())
		throw std::runtime_error{"date string contains extra characters"};
	// convert day
	// TODO: kind of inefficient to create a new temporary string
	auto d = std::stoi(std::string{n_begin, n_end});
	// check that number of days is valid
	// note: DaysInMonth() will throw if the month value is invalid
	if (!d || d > DaysInMonth(y, m))
		throw std::runtime_error{"invalid day number " + std::to_string(d)};
	// convert to Julian date
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
	// FIXME: may need a +1 according to Wikipedia
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
	auto [y, m, d] = YMD(julian_);
#if OA_HAS_CPP20_FORMAT
	return std::format("{}-{}-{} : Julian Integer = {}", y, m, d, julian_);
#else
	return
		std::to_string(y) + "-" + std::to_string(m) + "-" + std::to_string(d) +
		" : Julian Integer = " + std::to_string(julian_);
#endif  // !OA_HAS_CPP20_FORMAT
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

unsigned
Date::DaysInMonth(int year, unsigned month)
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
		//added this for when you are in june add 6M then land in decmebr but the year counter increments :S
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
	// TODO: deal with narrowing issue
	return Date(new_year, new_month, new_day);
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
	// TODO: deal with narrowing issue
	return Date(year, month, day);
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
	return julian_ < 0;
}

Date
Date::value_or(Date other) const noexcept
{
	// note: 0 is technically still a valid Julian day number
	return (julian_ > 0) ? *this : other;
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
