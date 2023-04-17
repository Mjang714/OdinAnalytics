#include "adjustment_factory.h"


namespace oa::time
{
	std::unique_ptr<DateAdjustBase> AdjustmentFactory::CreateDateAdjust(const AdjRule biz_day_convnetion, const Calendar& given_calendar)
	{
		switch (biz_day_convnetion)
		{
			case AdjRule::kFollowing:
				return std::make_unique<DateAdjustFollowing>(given_calendar);
			case AdjRule::kPreceding:
				return std::make_unique<DateAdjustPreceding>(given_calendar);
			case AdjRule::kModifiedFollowing:
				return std::make_unique<DateAdjustModFollowing>(given_calendar);
			default:
				return CreateDateAdjust(biz_day_convnetion);
		}
	}
	std::unique_ptr<DateAdjustBase> AdjustmentFactory::CreateDateAdjust(const AdjRule biz_day_convnetion)
	{
		if (biz_day_convnetion == AdjRule::kPlainAdjustment)
		{
			return std::make_unique<DateAdjustPlain>();
		}

		else
		{
			// default return nullptr when we have nothing
			return nullptr;
		}
	}
}