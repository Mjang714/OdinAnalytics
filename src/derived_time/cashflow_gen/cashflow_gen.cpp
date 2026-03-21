#include "cashflow_gen.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

// TODO: centralize includes
#include "derived_time/cashflow_gen/cashflow_struct.h"
#include "derived_time/derived_time_enums.h"
#include "derived_time/date_formula/business_date_formula.h"
#include "helpers/utils.h"
#include "time/date_adjust/date_adjust_base.h"
#include "time/day_count/day_counter_factory.h"
#include "time/tenor.h"
#include "oa/ctti.h"
#include "oa/platform.h"

namespace oa::derived_time {

////////////////////////////////////////////////////////////////////////////////
// CashflowGen::Options                                                       //
////////////////////////////////////////////////////////////////////////////////

// see Options() declaration for why an explicit definition is required
CashflowGen::Options::Options() noexcept {
	// set defaults for optional parameters
	currency_ = Currency::kUSD;
	date_dir_ = DateDirection::kBackward;
	cashflow_type_ = CashflowType::kFixed;
	reset_dir_ = ResetDirection::kAdvance;
	stub_type_ = StubType::kNone;
	start_adj_ = BusinessDateFormula{};
	end_adj_ = BusinessDateFormula{};
	pay_adj_ = BusinessDateFormula{};
	fix_adj_ = BusinessDateFormula{};
	stub_date_ = time::Date{};
	calc_type_ = CalcType::kFlat;
}

Currency
CashflowGen::Options::currency() const noexcept
{
	return currency_;
}

CashflowGen::Options&
CashflowGen::Options::currency(Currency ccy) noexcept
{
	currency_ = ccy;
	return *this;
}

DateDirection
CashflowGen::Options::date_direction() const noexcept
{
	return date_dir_;
}

CashflowGen::Options&
CashflowGen::Options::date_direction(DateDirection dir) noexcept
{
	date_dir_ = dir;
	return *this;
}

CashflowType
CashflowGen::Options::cashflow_type() const noexcept
{
	return cashflow_type_;
}

CashflowGen::Options&
CashflowGen::Options::cashflow_type(CashflowType type) noexcept
{
	cashflow_type_ = type;
	return *this;
}

ResetDirection
CashflowGen::Options::reset_direction() const noexcept
{
	return reset_dir_;
}

CashflowGen::Options&
CashflowGen::Options::reset_direction(ResetDirection dir) noexcept
{
	reset_dir_ = dir;
	return *this;
}

StubType
CashflowGen::Options::stub_type() const noexcept
{
	return stub_type_;
}

CashflowGen::Options&
CashflowGen::Options::stub_type(StubType type) noexcept
{
	stub_type_ = type;
	return *this;
}

const BusinessDateFormula&
CashflowGen::Options::start_adjustment() const noexcept
{
	return start_adj_;
}

CashflowGen::Options&
CashflowGen::Options::start_adjustment(BusinessDateFormula adj)
{
	start_adj_ = std::move(adj);
	return *this;
}

const BusinessDateFormula&
CashflowGen::Options::end_adjustment() const noexcept
{
	return end_adj_;
}

CashflowGen::Options&
CashflowGen::Options::end_adjustment(BusinessDateFormula adj)
{
	end_adj_ = std::move(adj);
	return *this;
}

const BusinessDateFormula&
CashflowGen::Options::pay_adjustment() const noexcept
{
	return pay_adj_;
}

CashflowGen::Options&
CashflowGen::Options::pay_adjustment(BusinessDateFormula adj)
{
	pay_adj_ = std::move(adj);
	return *this;
}

const BusinessDateFormula&
CashflowGen::Options::fix_adjustment() const noexcept
{
	return fix_adj_;
}

CashflowGen::Options&
CashflowGen::Options::fix_adjustment(BusinessDateFormula adj)
{
	fix_adj_ = std::move(adj);
	return *this;
}

time::Date
CashflowGen::Options::stub_date() const
{
	return stub_date_;
}

CashflowGen::Options&
CashflowGen::Options::stub_date(time::Date date)
{
	stub_date_ = date;
	return *this;
}

CalcType
CashflowGen::Options::calc_type() const
{
	return calc_type_;
}

CashflowGen::Options&
CashflowGen::Options::calc_type(CalcType type)	
{
	calc_type_ = type;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// CashFlowGen                                                                //
////////////////////////////////////////////////////////////////////////////////
	std::vector<CashflowStruct> CashflowGen::CreateFixedCashflows(
		const time::Date& start_date,
		const time::Date& mat_date,
		const time::Tenor& reset_freq,
		const double notional,
		const double rate,
		const time::DayCountRule day_count_rule,
		const Options& opts)
	{
		if(start_date > mat_date)
			throw std::invalid_argument(
#if OA_HAS_CPP20_FORMAT 
				std::format("{}:{} - Start date {} is before maturity date {}", std::string{OA_SOURCE_LOCATION()} , __func__, start_date.ToString(), mat_date.ToString())
#else
				std::string{OA_SOURCE_LOCATION()} + ":" +
				__func__ + ": " + "Start date " + start_date.ToString() + " is before maturity date " + mat_date.ToString()
#endif
				);
		std::vector<CashflowStruct> cashflows{};
		std::vector<time::Date> unadjusted_start_dates{};
		std::vector<time::Date> unadjusted_end_dates{};
		auto time_length = reset_freq.GetValues().first;
		auto tenor_enum = reset_freq.GetValues().second;

		if (opts.date_direction() == DateDirection::kForward) {
			auto curr_start_date = start_date;
			auto total_length = time_length;
			while (curr_start_date < mat_date) {

				if (curr_start_date < mat_date) {
					unadjusted_start_dates.emplace_back(curr_start_date);
					unadjusted_end_dates.emplace_back(curr_start_date.AddTenor({time_length, tenor_enum}));
				}
				curr_start_date = start_date.AddTenor({total_length, tenor_enum});
				total_length += time_length;
			}
		}

		else {
			auto curr_end_date = mat_date;
			auto total_length = -time_length;
			while (curr_end_date > start_date) {
				if (curr_end_date > start_date) {
					unadjusted_end_dates.emplace_back(curr_end_date);
					unadjusted_start_dates.emplace_back(curr_end_date.AddTenor({-time_length, tenor_enum}));
				}
				curr_end_date = mat_date.AddTenor({total_length, tenor_enum});
				total_length -= time_length;
			}
			//this ensure that the cashflows dates are in the same chonological order.
			std::reverse(unadjusted_start_dates.begin(), unadjusted_start_dates.end());
			std::reverse(unadjusted_end_dates.begin(), unadjusted_end_dates.end());
		}


		StubDateAdjustments(start_date, mat_date, unadjusted_start_dates, unadjusted_end_dates, opts);

		cashflows.reserve(unadjusted_start_dates.size() + 1); // +1 for the final principal repayment
		auto day_count = time::DayCounterFactory::GenerateDayCounter(day_count_rule);
		for (size_t i = 0; i < unadjusted_start_dates.size(); i++) {
			CashflowStruct cf{};
			cf.unadj_start_date = unadjusted_start_dates[i];
			cf.unadj_end_date = unadjusted_end_dates[i];
			cf.start_date = cf.unadj_start_date + opts.start_adjustment();
			cf.end_date = cf.unadj_end_date + opts.end_adjustment();
			cf.payment_date = cf.end_date + opts.pay_adjustment();
			cf.notional = notional;
			cf.rate = rate;
			cf.days = day_count->DayCount(cf.start_date, cf.end_date);
			cf.cf_type = opts.cashflow_type();
			cf.day_count_fraction = day_count->YearFraction(cf.start_date, cf.end_date);
			cf.cf_curr = opts.currency();

			// no fixing adjustment -- use start date
			if (!opts.fix_adjustment())
				cf.fixing_date = cf.start_date;
			// otherwise, if advance reset, adjust start date
			else if (opts.reset_direction() == ResetDirection::kAdvance)
				cf.fixing_date = cf.start_date + opts.fix_adjustment();
			// otherwise, adjust end date
			else
				cf.fixing_date = cf.end_date + opts.fix_adjustment();
			
			ComputeCoupon(cf, reset_freq, opts);
			// using emplace_back and std::move though not sure if it is necessary here
			cashflows.emplace_back(std::move(cf));
		}
		//add the princple to the last cashflow

		CashflowStruct cf(cashflows.back());
		cf.cashflow_amount = notional;
		cf.rate = 1.0;
		cf.cf_type = CashflowType::kPrincipal;
		cashflows.emplace_back(std::move(cf));
		// Implementation logic to generate cashflows goes here
		return cashflows;
	}


	std::vector<CashflowStruct> CashflowGen::CreateFixedCashflows(
		const time::Date& start_date,
		const time::Date& mat_date,
		const Frequency& reset_freq,
		const double notional,
		const double rate,
		const time::DayCountRule day_count_rule,
		const Options& opts)
	{

		auto tenor_freq = MapResetFreqEnumToTenor(reset_freq);
		return CreateFixedCashflows(start_date, mat_date, tenor_freq, notional, rate, day_count_rule, opts);

	}

	void CashflowGen::ComputeCoupon(
		CashflowStruct& cf, 
		const time::Tenor& reset_freq, 
		const Options& opts)
	{
		switch(opts.calc_type())
		{
			case CalcType::kFlat:
				cf.cashflow_amount = cf.notional * cf.rate * cf.day_count_fraction;
				break;
			case CalcType::kBBGCalcType1:
				[[fallthrough]];
			case CalcType::kBBGCalcType2:
				//this is really for when issue bond but the general coupon computations are the same so I am putting it in the same place for now. This can be refactored later if needed.
				[[fallthrough]];
			case CalcType::kUSTStreetConv:
				cf.cashflow_amount = cf.notional * (cf.rate / MapResetFreqEnumToInt(reset_freq));
				break;
			default:
				throw std::invalid_argument("Invalid calculation type provided");
		}
	}

	void CashflowGen::StubDateAdjustments(
		const oa::time::Date& start_date,
		const oa::time::Date& mat_date,
		std::vector<oa::time::Date>& unadjusted_start_dates,
		std::vector<oa::time::Date>& unadjusted_end_dates,
		const Options& opts
	) 
	{
		// stub adjustment logic
		// TODO: handle invalid combinations of stub type + date direction
		switch (opts.date_direction()) {
			// backwards date direction
		case DateDirection::kBackward:
			switch (opts.stub_type()) {
			case StubType::kLongFirst:
				unadjusted_start_dates.erase(unadjusted_start_dates.begin());
				unadjusted_end_dates.erase(unadjusted_end_dates.begin());
				[[fallthrough]];
			case StubType::kShortFirst:
				unadjusted_start_dates.front() = opts.stub_date().value_or(start_date);
				break;
			default:
				break;
			}
			break;
			// forward date direction
		case DateDirection::kForward:
			switch (opts.stub_type()) {
			case StubType::kLongLast:
				unadjusted_end_dates.pop_back();
				unadjusted_start_dates.pop_back();
				[[fallthrough]];
			case StubType::kShortLast:
				unadjusted_end_dates.back() = opts.stub_date().value_or(mat_date);
				break;
			default:
				break;
			}
			break;
		}
	}
	
	time::Tenor CashflowGen::MapResetFreqEnumToTenor(const Frequency reset_freq)
	{
		const static std::unordered_map<Frequency, time::Tenor> reset_freq_enum_to_tenor{
			{Frequency::kAnnual, time::Tenor("1Y")},
			{Frequency::kSemiAnnual,time::Tenor("6M")},
			{Frequency::kQuarterly, time::Tenor("3M")},
			{Frequency::kMonthly, time::Tenor("1M")},
			{Frequency::kWeekly, time::Tenor("1W")},
			{Frequency::kDaily, time::Tenor("1D")}
		};

		return reset_freq_enum_to_tenor.at(reset_freq);
	}

	// I am open to refactor for this in anohter branch.... clearly cause we are doing this indicates I was myopic about this (facepalm) 
	// I think we should have sticked to frequency being enum.....
	int CashflowGen::MapResetFreqEnumToInt(const oa::time::Tenor reset_freq, bool is_leap_year)
	{
		
		switch (reset_freq.GetValues().second)
		{
		case oa::time::Tenors::kYears:
			//if it is in years best to return 1 can't think of any bond that pay every year a bit nonsensical
			return reset_freq.GetValues().first;
		case oa::time::Tenors::kMonths:
			return 12 / reset_freq.GetValues().first;
		case oa::time::Tenors::kWeeks:
			return 52 / reset_freq.GetValues().first;
		case oa::time::Tenors::kDays:
			return (is_leap_year ? 366 : 365) / reset_freq.GetValues().first;	
		default:
			throw std::invalid_argument("Unsupported reset frequency for calculation type provided");
		}
		
	}

}  // namespace oa::derived_time
		