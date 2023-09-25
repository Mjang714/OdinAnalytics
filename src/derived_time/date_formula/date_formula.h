#ifndef ODINANALYTICS_DERIVED_TIME_DATE_FORMULA_H_
#define ODINANALYTICS_DERIVED_TIME_DATE_FORMULA_H_

#include <memory>
#include <string>

#include "oa/dllexport.h"
#include "time/calendar.h"
#include "time/tenor.h"

namespace oa::derived_time
{
	class OA_DERIVED_TIME_API DateFormula
	{
		public:
			DateFormula();
			DateFormula(const oa::time::Tenor& tenor_obj, oa::time::AdjRule adjustment_rule, const oa::time::Calendar& calendar);
			DateFormula(const std::string& tenor_obj,  oa::time::AdjRule adjustment_rule, const oa::time::Calendar& calendar);

		private:

	};
}

#endif // !ODINANALYTICS_DERIVED_TIME_DATE_FORMULA_H_





