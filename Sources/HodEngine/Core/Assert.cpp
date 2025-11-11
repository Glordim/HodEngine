#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Assert.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod
{
	bool _reportingAssert = false;

	void ReportAssert(const char* condition, const char* file, int line)
	{
		OutputService::AddOutput(file, line, hod::Output::Type::Error, condition);
	}
}
