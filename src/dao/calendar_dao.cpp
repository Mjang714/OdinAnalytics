#include "calendar_dao.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "helpers/str_utils/str_utils.h"
#include "helpers/utils.h"
#include "oa/platform.h"
#include "oa/rtti.h"

#if OA_HAS_CPP20_FORMAT
#include <format>
#endif  // !OA_HAS_CPP20_FORMAT

namespace oa::dao
{

	CalendarDao& CalendarDao::GetInstance()
	{
		static CalendarDao calendar_dao_instance{};
		return calendar_dao_instance;
	}

	ds::CalendarDataStruct CalendarDao::GetCalendartData(const std::string& region)
	{
		ds::CalendarDataStruct calendar_struct{};
		// construct path to corresponding calendar file
		auto cal_file_path = [&region]
		{
			// if no OdinBaseDir throw
			// TODO: consider use of oa_config library to construct path
			// relative to oa::config::data_dir() after build system is updated
			// to copy the static_data directory to the build directory
			auto base_dir = std::getenv("OdinBaseDir");
			// note: could use fixed_string for compile-time string concat
			if (!base_dir)
				throw std::runtime_error{
					OA_PRETTY_FUNCTION_NAME +
					std::string{": missing required OdinBaseDir"}
				};
			// otherwise, construct path to calendar file
			return std::filesystem::path{base_dir} /
				"static_data" /
				"calendars" /
				(region + ".hol");
		}();
		// regular file check
		// note: indirectly also checks for existence
		if (!std::filesystem::is_regular_file(cal_file_path))
		{
			throw std::invalid_argument{
#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}",
					"File not found "+ region + ".hol",
					"calendar_dao.cpp line 22 GetCalendarData()"
				)
#else
				"File not found " + region + ".hol:" + std::string{__FILE__} +
				"line " + std::to_string(__LINE__) + " " + std::string{__func__}
#endif  // !OA_HAS_CPP20_FORMAT
			};
		}

		std::ifstream raw_calendar_data{ cal_file_path};

		if (!raw_calendar_data.is_open())
		{
			throw std::runtime_error{
#if OA_HAS_CPP20_FORMAT
				std::format(
					"{}:{}",
					"Calendar file cannot be open " + region + ".hol",
					"calendar_dao.cpp line 31 GetCalendarData()"
				)
#else
				"Calendar file cannot be open " + region + ".hol:" +
				std::string{__FILE__} + " line " + std::to_string(__LINE__) +
				" " + std::string{__func__}
#endif  // !OA_HAS_CPP20_FORMAT
			};
		}

		std::string file_line{};

		while (std::getline(raw_calendar_data, file_line))
		{
			if (file_line == weekend_delimiter)
			{
				//get the next line of the file
				std::getline(raw_calendar_data, file_line);
				for (auto weekend_integer : utils::str_utils::StrToDigits(file_line))
				{
					if (weekend_integer > -1 && weekend_integer < 7)
					{
						calendar_struct
							.weekends
							.push_back(utils::ValueToEnum<time::Weekdays>(weekend_integer));
					}
					else
					{
						throw std::runtime_error{
#if OA_HAS_CPP20_FORMAT
							std::format(
								"{}:{}",
								region + ".hol has an invalid weekend integer " + file_line,
								"calendar_dao.cpp line 52 GetCalendarData()"
							)
#else
							region + ".hol has an invalid weekend integer " + file_line +
							std::string{__FILE__} + " line " + std::to_string(__LINE__) +
							" " + std::string{__func__}
#endif  // !OA_HAS_CPP20_FORMAT
						};
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

}  // namespace oa::dao
