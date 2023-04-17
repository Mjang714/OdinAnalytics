#ifndef ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTBASE_H
#define ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTBASE_H

#include "time/date.h"

namespace oa::time
{

	class DateAdjustBase
	{
		public: 
			virtual ~DateAdjustBase() = default;

			virtual oa::time::Date AdjustDate(const oa::time::Date& given_date) const = 0;
	};
}

#endif	//ODINANALYTICS_TIME_DATEADJUST_DATEADJUSTBASE_H

