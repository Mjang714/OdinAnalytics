#include "date.h"

namespace oa::time
{


	Date::Date(const int& year, const int& month, const int& day) : m_days_(day), m_months_(month), m_years_(year), m_julian_int_(ConvertToJulian(year, month, day))
	{

	}

	Date::Date(const std::string& date_str)
	{
		std::vector <std::string> split_date_str{};

		if (oa::utils::CheckDateStr(date_str))
		{
			boost::split(split_date_str, date_str, boost::is_any_of("\\-\\/:"));
			this->m_years_ = std::stoi(split_date_str.at(0)); 
			this->m_months_ = std::stoi(split_date_str.at(1)); 
			this->m_days_ = std::stoi(split_date_str.at(2));
			this->m_julian_int_ = ConvertToJulian(m_years_, m_months_, m_days_);
		}

		else
		{
			throw "Invalid date string please check your string input!: " + date_str;
		}

	}

	Date::Date(const int& julian_int)
	{

		this->m_julian_int_ = julian_int;
		std::tuple<int, int, int> results = this->ConvertToGregInt(julian_int);
		PopulateYMDFromTuple(results);

	}

	Date::Date(const std::chrono::system_clock::time_point& time_point)
	{
		std::tuple<int, int, int> results = ConvertToGregInt(time_point);
		PopulateYMDFromTuple(results);
		this->m_julian_int_ = ConvertToJulian(this->m_years_, this->m_months_, this->m_days_);
	}

	void Date::PopulateYMDFromTuple(const std::tuple<int, int, int>& results)
	{
		this->m_years_ = std::get<0>(results);
		this->m_months_ = std::get<1>(results);
		this->m_days_ = std::get<2>(results);
	}

	std::tuple<int, int, int> Date::ConvertToGregInt(const std::chrono::system_clock::time_point& time_point)
	{
		auto days_point = floor<std::chrono::days> (time_point);
		std::chrono::year_month_day ymd{ days_point };

		//this will do an implicit cast of unsigned ints to int which is not ideal....  
		int year = static_cast<int> (ymd.year());
		int month = static_cast<unsigned> (ymd.month());
		int day = static_cast<unsigned> (ymd.day());
			
		return std::tuple<int, int, int>(year,month,day);
	}

	std::tuple<int, int, int> Date::ConvertToGregInt(const int & julian_day) const
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

		std::tuple<int, int, int> ymd_result(computed_years, computed_months, computed_days);
		return ymd_result;
	}


	int Date::ConvertToJulian(const int& year, const int& month, const int& day)
	{
		return (1461 *(year + 4800 + (month - 14) / 12)) / 4 + 
			(367 *(month - 2 - 12 *((month - 14) / 12))) / 12 -
			(3 *((year + 4900 + (month - 14) / 12) / 100)) / 4 
			+ day - 32075;

	}

	int Date::GetJulian() const
	{
		return m_julian_int_;
	}

	int Date::GetDOWInt()
	{
		return GetDOWInt(m_julian_int_);
	}

	int Date::GetDOWInt(int julian_date)
	{
		return julian_date % 7;
	}



		
	std::chrono::system_clock::time_point Date::ConvertToTimePt(void)
	{
		return ConvertToTimePt(*this);
	}

	std::chrono::system_clock::time_point Date::ConvertToTimePt(const Date& given_date) 
	{
		std::tm time_point_result {};

		time_point_result.tm_year = given_date.m_years_ - 1900;
		time_point_result.tm_mon = given_date.m_months_ - 1;
		time_point_result.tm_mday = given_date.m_days_;
		time_point_result.tm_hour = 0;
		time_point_result.tm_min = 0;
		time_point_result.tm_sec = 0;
		time_point_result.tm_isdst = -1;

		return std::chrono::system_clock::from_time_t(std::mktime(&time_point_result));
	}

	std::string Date::ToString()
	{
		return std::to_string(m_years_) + "-" + std::to_string(m_months_) + "-" + std::to_string(m_days_) 
			+ " : Julian Integer = " + std::to_string(m_julian_int_);
	}

	bool Date::IsLeap()
	{
		return IsLeap(m_years_);
	}

	bool Date::IsLeap( int year) 
	{
		return  (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
	}

	int Date::DaysInMonth( int month, int year)
	{	

		if (month == 2)
		{
			return (IsLeap(year)) ? 29 : 28;
		}
		else if (month == 4 || month == 6 || month == 9 || month == 11)
		{
			return 30;
		}

		else 
		{
			return 31;
		}
	}

	Date Date::SubTenor(const oa::time::Tenor& tenor)
	{
		return AddTenor(tenor.FlipSign());
	}

	Date Date::AddTenor(const oa::time::Tenor& tenor)
	{
		auto values = tenor.GetValues();
	
		switch(values.second)
		{
			case oa::time::Tenors::kDays:
				return AddDays(values.first);

			case oa::time::Tenors::kWeeks:
				return AddWeeks(values.first);

			case oa::time::Tenors::kMonths:
				return AddMonths(values.first);

			case oa::time::Tenors::kYears:
				return AddYears(values.first);

			default:
				throw "Not a valid Tenor please check input tenor of:" + oa::utils::GetCleanName(values.second);
		}
	}

	Date Date::AddDays(const int& time_length) const
	{
		return Date(GetJulian() + time_length);
	}

	Date Date::AddWeeks(const int& time_length) const
	{
		return Date(GetJulian() + time_length * 7);
	}

	Date Date::AddMonths(const int& time_length) const
	{
		int new_day(0), new_month(0), new_year(0);
			
		//compute the numbers of years 
		if (time_length > 0)
		{
			new_year = m_years_ + (m_months_ + time_length) / 12;
			new_month = (m_months_ + time_length) % 12;

		}

		else
		{
			new_year = m_years_ + (m_months_ + time_length) / 12;
			new_month = (m_months_ + time_length) % 12;
			if (new_month < 1)
			{
				new_year += -1;
			}
		}
			
		//check for when we add negative dates
		if (new_month <= 0)
		{
			new_month = new_month + 12;
		}

		//logic to check the day part is right
		new_day = m_days_;
		if (new_day > DaysInMonth(new_month, new_year))
		{
			new_day = DaysInMonth(new_month, new_year);
		}

		return Date(new_year, new_month, new_day);
	}

	Date Date::AddYears(const int& time_length) const
	{
		int new_day(0), new_month(0), new_year(0);
		new_year = m_years_ + time_length;
		new_month = m_months_;
		new_day = m_days_;

		//if it is not leap year then check to see if this 
		if (!IsLeap(new_year))
		{
			//if it's february check we have teh right 
			if (new_month == 2)
			{
				new_day = (new_day > DaysInMonth(new_month, new_year)) ? DaysInMonth(new_month, new_year) : new_day;
			}
		}

		return Date(new_year, new_month, new_day);
	}

	Date Date::SubDays(const int& time_length) const
	{
		return AddDays(-time_length);
	}

	Date Date::SubWeeks(const int& time_length) const
	{
		return AddWeeks(-time_length);
	}

	Date Date::SubMonths(const int& time_length) const
	{
		return AddMonths(-time_length);
	}

	Date Date::SubYears(const int& time_length) const
	{
		return AddYears(-time_length);
	}

	//defining operator overlaoding 
	bool Date::operator==(const Date& right_value) const
	{
		return m_julian_int_ == right_value.m_julian_int_;
	}

	bool Date::operator!=(const Date& right_value) const
	{
		return m_julian_int_ != right_value.m_julian_int_;
	}

	bool Date::operator<=(const Date& right_value) const
	{
		return m_julian_int_ <= right_value.m_julian_int_;
	}

	bool Date::operator>=(const Date& right_value) const
	{
		return m_julian_int_ >= right_value.m_julian_int_;
	}

	bool Date::operator<(const Date& right_value) const
	{
		return m_julian_int_ < right_value.m_julian_int_;
	}

	bool Date::operator>(const Date& right_value) const
	{
		return m_julian_int_ > right_value.m_julian_int_;
	}
	
}