#pragma once
#include "HodEngine/Core/Export.hpp"

namespace hod
{
	class HOD_CORE_API ProfilerScopedEvent
	{
	public:
		ProfilerScopedEvent(const char* name);
		~ProfilerScopedEvent();
	};

	void HOD_CORE_API ProfilerBeginEvent(const char* name);
	void HOD_CORE_API ProfilerEndEvent();
}

#define PROFILER_SCOPED_EVENT(Name) hod::ProfilerScopedEvent profilerScopedEvent(Name);
#define PROFILER_BEGIN_EVENT(Name) hod::ProfilerBeginEvent(Name);
#define PROFILER_END_EVENT() hod::ProfilerEndEvent();
