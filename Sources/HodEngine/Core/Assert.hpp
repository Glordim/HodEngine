#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Debug.hpp"

namespace hod
{
	// Macro ensures the crash triggers in caller code
#define Crash()                \
	int* invalidPtr = nullptr; \
	*invalidPtr = 0;

	// Avoid nested assert (from Output)
	HOD_CORE_API extern bool _reportingAssert;

	HOD_CORE_API void ReportAssert(const char* condition, const char* file, int line);

	// Macro ensures the assert triggers in caller code
#define Assert(condition)                             \
	if (!(condition) && _reportingAssert == false)    \
	{                                                 \
		_reportingAssert = true;                      \
		ReportAssert(#condition, __FILE__, __LINE__); \
		if (IsDebuggerAttached())                     \
			Break();                                  \
		Crash();                                      \
	}
}
