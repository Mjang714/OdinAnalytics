#ifndef OA_DERIVED_TIME_CAHFLOW_STRUCT_H_
#define OA_DERIVED_TIME_CAHFLOW_STRUCT_H_
#include <memory>
#include <ostream>
#include <string>

#include "oa/dllexport.h"
#include "time/date.h"
#include "helpers/utils.h"
#include "derived_time/derived_time_enums.h"


namespace date = oa::time;

namespace oa::derived_time {
	struct OA_DERIVED_TIME_API CashflowStruct {
		date::Date unadj_start_date;
		date::Date unadj_end_date;
		date::Date start_date;
		date::Date end_date;
		date::Date fixing_date;
		date::Date payment_date;
		double notional;
		double rate;
		double cashflow_amount;
		double npv_cashflow_amount;
		int days;
		double day_count_fraction;
		Currency cf_curr;
		CashflowType cf_type;

		CashflowStruct() = default;	

		CashflowStruct(
			const std::string& unadj_start_str,
			const std::string& unadj_end_str,
			const std::string& start_str,
			const std::string& end_str,
			const std::string& fixing_str,
			const std::string& payment_str,
			double inpt_notional,
			double input_rate,
			double input_cashflow_amount,
			double input_npv_cashflow_amount,
			int input_days,
			double input_day_count_fraction,
			Currency input_cf_curr,
			CashflowType input_cf_type) :
			unadj_start_date(unadj_start_str),
			unadj_end_date(unadj_end_str),
			start_date(start_str), 
			end_date(end_str),
			fixing_date(fixing_str),
			payment_date(payment_str),
			notional(inpt_notional),
			rate(input_rate),
			cashflow_amount(input_cashflow_amount),
			npv_cashflow_amount(input_npv_cashflow_amount),
			days(input_days),
			day_count_fraction(input_day_count_fraction),
			cf_curr(input_cf_curr),
			cf_type(input_cf_type)
			{

			}

		bool operator==(const CashflowStruct& right_value) const = default;

		
	};

	inline void PrintTo(const CashflowStruct& cf, std::ostream* os)
	{
		*os << "CashflowStruct{\n"
			<< "unadj_start_date = " << cf.unadj_start_date.ToString() << "\n"
			<< "unadj_end_date = " << cf.unadj_end_date.ToString() << "\n"
			<< "start_date = " << cf.start_date.ToString() << "\n"
			<< "end_date = " << cf.end_date.ToString() << "\n"
			<< "fixing_date = " << cf.fixing_date.ToString() << "\n"
			<< "payment_date = " << cf.payment_date.ToString() << "\n"
			<< "notional = " << cf.notional << "\n"
			<< "rate = " << cf.rate << "\n"
			<< "cashflow_amount = " << cf.cashflow_amount << "\n"
			<< "npv_cashflow_amount = " << cf.npv_cashflow_amount << "\n"
			<< "days = " << cf.days << "\n"
			<< "day_count_fraction = " << cf.day_count_fraction << "\n"
			<< "cf_curr = " << oa::utils::EnumToInt<Currency>(cf.cf_curr) << "\n"
			<< "cf_type = " << oa::utils::GetCleanName<CashflowType>(cf.cf_type)
			<< "}";
	}
}
#endif //OA_DERIVED_TIME_CAHFLOW_STRUCT_H_


