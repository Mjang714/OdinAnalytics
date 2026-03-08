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

namespace oa::derived_time {

////////////////////////////////////////////////////////////////////////////////
// CashflowGen::Options                                                       //
////////////////////////////////////////////////////////////////////////////////

// see Options() declaration for why an explicit definition is required
CashflowGen::Options::Options() noexcept {}

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

////////////////////////////////////////////////////////////////////////////////
// CashFlowGen                                                                //
////////////////////////////////////////////////////////////////////////////////
	std::vector<CashflowStruct> CashflowGen::CreateFixedCashflows(
		const time::Date& start_date,
		const time::Date& mat_date,
		const time::Tenor reset_freq,
		const double notional,
		const double rate,
		const time::DayCountRule day_count_rule,
		const Options& opts)
	{
		
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


		for (size_t i = 0; i < unadjusted_start_dates.size(); i++) {
			auto day_count = time::DayCounterFactory::GenerateDayCounter(day_count_rule);
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
			cf.cashflow_amount = notional * (rate * cf.day_count_fraction);
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
		const Frequency reset_freq,
		const double notional,
		const double rate,
		const time::DayCountRule day_count_rule,
		const Options& opts)
	{

		auto tenor_freq = MapResetFreqEnumToTenor(reset_freq);
		return CreateFixedCashflows(start_date, mat_date, tenor_freq, notional, rate, day_count_rule, opts);

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

}  // namespace oa::derived_time