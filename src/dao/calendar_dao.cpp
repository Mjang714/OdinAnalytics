#include "calendar_dao.h"


namespace oa::dao
{

	CalendarDao& CalendarDao::GetInstance()
	{
		static CalendarDao calendar_dao_instance{};
		return calendar_dao_instance;
	}

	cal_struct::CalendarDataStruct CalendarDao::GetCalendartData(const std::string& region)
	{
		cal_struct::CalendarDataStruct calendar_struct{};
		std::string calendar_file_path_str = std::getenv("OdinBaseDir") + std::string("//static_data//calendars//");
		std::filesystem::path cal_file_path{ calendar_file_path_str + region + ".hol" };
		if (!std::filesystem::is_regular_file(cal_file_path))
		{
			throw std::invalid_argument(
				std::format("{}:{}", "File not found "+ 
					region + ".hol", "calendar_dao.cpp line 22 GetCalendarData()"));
		}

		std::ifstream raw_calendar_data{ cal_file_path};

		if (!raw_calendar_data.is_open())
		{
			throw std::runtime_error(
				std::format("{}:{}", "Calendar file cannot be open " +
					region + ".hol", "calendar_dao.cpp line 31 GetCalendarData()"));
		}

		std::string file_line{}; 

		while (std::getline(raw_calendar_data, file_line))
		{
			if (file_line == weekend_delimiter)
			{
				//get eh next line of the file
				std::getline(raw_calendar_data, file_line);
				for (auto weekend_integer : str_helpers::StrToDigits(file_line))
				{
					if (weekend_integer > -1 && weekend_integer < 7)
					{
						calendar_struct.weekends.push_back(helpers::ValueToEnum<oa::time::Weekdays>(weekend_integer));
					}
					else
					{
						throw std::runtime_error(
								std::format("{}:{}", region + ".hol has an invalid weekend integer "
									+ file_line, "calendar_dao.cpp line 52 GetCalendarData()"));
					}
				}
			}

			else if (file_line == holidays_delimiter)
			{
				while (std::getline(raw_calendar_data, file_line))
				{
					calendar_struct.list_of_holidays.push_back(oa::time::Date(file_line));
				}
			}
		}
		raw_calendar_data.close();
		calendar_struct.regions = region;
		return calendar_struct;
	}
}