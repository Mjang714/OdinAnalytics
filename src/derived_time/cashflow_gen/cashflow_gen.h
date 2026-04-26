#ifndef OA_DERIVED_TIME_CASHFLOW_GEN_CASHFLOW_GEN_H_
#define OA_DERIVED_TIME_CASHFLOW_GEN_CASHFLOW_GEN_H_

#include <vector>

#include "oa/dllexport.h"
#include "derived_time/cashflow_gen/cashflow_struct.h"
#include "derived_time/date_formula/business_date_formula.h"
#include "derived_time/derived_time_enums.h"
#include "time/date.h"

namespace oa::derived_time {

	class OA_DERIVED_TIME_API CashflowGen {
	public:
		/**
		 * Options class for cashflow generation.
		 *
		 * This provides a fluent API to make setting values transparent.
		 */
		class OA_DERIVED_TIME_API Options {
		public:
			/**
			 * Default ctor.
			 *
			 * Surprisingly, it is technically required to explicitly define
			 * `Options()` as being default-constructible to use it in
			 * `CreateFixedCashflows()` below. This is because the nested
			 * `Options` class is considered *incomplete* until the enclosing
			 * `CashflowGen` class is complete. Therefore, using `{}` to
			 * initialize a default in `CreateFixedCashflows()` is use of an
			 * incomplete type, which triggers a compiler error.
			 *
			 * This is surprising behavior because using `= default` results in
			 * the same error, although the reason is that using `default`
			 * means the compiler needs to define `Options()`, and therefore
			 * needs to complete the type before making this determination.
			 * Notably, MSVC is happy with implicitly declaring + defining the
			 * `Options()` default constructor, while both GCC and Clang
			 * require us to provide an explicit `Options()` definition.
			 *
			 * StackOverflow: https://stackoverflow.com/q/53408962/14227825
			 * LLVM GitHub: https://github.com/llvm/llvm-project/issues/36032
			 * GCC Bugzilla: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165
			 *
			 * Jonathan Wakely's comment is the reference for the above:
			 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165#c10
			 */
			Options() noexcept;

			/**
			 * Return the cash flow currency.
			 */
			Currency currency() const noexcept;

			/**
			 * Update the cash flow currency.
			 */
			Options& currency(Currency ccy) noexcept;

			/**
			 * Return the date direction.
			 */
			DateDirection date_direction() const noexcept;

			/**
			 * Update the cash flow date direction.
			 */
			Options& date_direction(DateDirection dir) noexcept;

			/**
			 * Return the cash flow type.
			 */
			CashflowType cashflow_type() const noexcept;

			/**
			 * Update the cash flow type.
			 */
			Options& cashflow_type(CashflowType type) noexcept;

			/**
			 * Return the reset direction.
			 */
			ResetDirection reset_direction() const noexcept;

			/**
			 * Update the reset direction.
			 */
			Options& reset_direction(ResetDirection dir) noexcept;

			/**
			 * Return the stub type.
			 */
			StubType stub_type() const noexcept;

			/**
			 * Update the stub type.
			 */
			Options& stub_type(StubType type) noexcept;

			/**
			 * Return the start date adjustment.
			 */
			const BusinessDateFormula& start_adjustment() const noexcept;

			/**
			 * Update the start date adjustment.
			 */
			Options& start_adjustment(BusinessDateFormula adj);

			/**
			 * Return the end date adjustment.
			 */
			const BusinessDateFormula& end_adjustment() const noexcept;

			/**
			 * Update the end date adjustment.
			 */
			Options& end_adjustment(BusinessDateFormula adj);

			/**
			 * Return the payment date adjustment.
			 */
			const BusinessDateFormula& pay_adjustment() const noexcept;

			/**
			 * Update the payment date adjustment.
			 */
			Options& pay_adjustment(BusinessDateFormula adj);

			/**
			 * Return the fixing date adjustment.
			 */
			const BusinessDateFormula& fix_adjustment() const noexcept;

			/**
			 * Update the fixing date adjustment.
			 */
			Options& fix_adjustment(BusinessDateFormula adj);

			/**
			 * Return the optional cash flow stub date.
			 */
			time::Date stub_date() const;

			/**
			 * Update the cash flow stub date.
			 */
			Options& stub_date(time::Date date);

			/**
			 * Return the calculation type for the cash flow amount.
			 */			
			CalcType calc_type() const;

			/**
			 * Update the calculation type for the cash flow amount.
			 */
			Options& calc_type(CalcType type);

		private:
			CalcType calc_type_{CalcType::kFlat};                  // calculation type
			Currency currency_{Currency::kUSD};                   // currency
			DateDirection date_dir_{DateDirection::kBackward};    // date direction
			CashflowType cashflow_type_{CashflowType::kFixed};    // cashflow type
			ResetDirection reset_dir_{ResetDirection::kAdvance};  // reset direction
			StubType stub_type_{StubType::kNone};                 // stub type
			time::Date stub_date_;                                // stub date
			BusinessDateFormula start_adj_;                       // start date adjustment
			BusinessDateFormula end_adj_;                         // end date adjustment
			BusinessDateFormula pay_adj_;                         // payment date adjustment
			BusinessDateFormula fix_adj_;                         // fixing date adjustment
		};

		static std::vector<CashflowStruct> CreateFixedCashflows(
			const time::Date& start_date,
			const time::Date& mat_date,
			const Frequency reset_freq,
			const double notional,
			const double rate,
			const time::DayCountRule day_count_rule,
			const Options& opts = {}
		);

		static std::vector<CashflowStruct> CreateFixedCashflows(
			const time::Date& start_date,
			const time::Date& mat_date,
			const time::Tenor& reset_freq,
			const double notional,
			const double rate,
			const time::DayCountRule day_count_rule,
			const Options& opts = {}
		);
		
		static oa::time::Tenor MapResetFreqEnumToTenor(
			const oa::derived_time::Frequency reset_freq
		);
	private:
		static int  DayCountDenominator(
			const oa::time::DayCountRule day_count_rule, 
			const bool is_leap_year = false
		);
		
		static double ComputeUSTStreetConvCoupon(
			const CashflowStruct& cf,
			const time::Tenor& reset_freq,
			const time::DayCountRule day_count_rule
		);

		static void ComputeCoupon(
			CashflowStruct& cf,
			const time::Tenor& reset_freq,
			const Options& opts,
			const time::DayCountRule day_count_rule
		);

		static void StubDateAdjustments(
			const oa::time::Date& start_date,
			const oa::time::Date& mat_date,
			std::vector<oa::time::Date>& unadjusted_start_dates,
			std::vector<oa::time::Date>& unadjusted_end_dates,
			const Options& opts
		);

	};
}

#endif // !OA_DERIVED_TIME_CASHFLOW_GEN_CASHFLOW_GEN_H_
