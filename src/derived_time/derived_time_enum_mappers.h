#ifndef OA__DERIVED_TIME_DERIVED_TIME_ENUM_MAPPERS_H_
#define OA__DERIVED_TIME_DERIVED_TIME_ENUM_MAPPERS_H_

#include "derived_time_enums.h"
#include "oa/dllexport.h"

namespace oa::derived_time
{
    OA_DERIVED_TIME_API
	derived_time::DateDirection MapInputToDateDir(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::Frequency MapInputToFreq(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::StubType MapInputToStub(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::CalcType MapInputToCalcType(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::DateDirection MapInputToDateDir(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::Frequency MapInputToFreq(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::StubType MapInputToStub(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::CalcType MapInputToCalcType(const std::string& input_str);
}

#endif  // OA__DERIVED_TIME_DERIVED_TIME_ENUM_MAPPERS_H_