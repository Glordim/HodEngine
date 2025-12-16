#pragma once
#include "HodEngine/Core/Export.hpp"

namespace hod
{
	class HOD_CORE_API ProfilerScopedEvent
	{
	public:
		ProfilerScopedEvent(const char* name, const char* param = nullptr);
		~ProfilerScopedEvent();
	};

	HOD_CORE_API void ProfilerBeginEvent(const char* name, const char* param = nullptr);
	HOD_CORE_API void ProfilerEndEvent();
}

#define PROFILER_SCOPED_EVENT(Name) hod::ProfilerScopedEvent profilerScopedEvent(Name);
#define PROFILER_SCOPED_EVENT_WITH_PARAM(Name, Param) hod::ProfilerScopedEvent profilerScopedEvent(Name, Param);
#define PROFILER_BEGIN_EVENT(Name) hod::ProfilerBeginEvent(Name);
#define PROFILER_BEGIN_EVENT_WITH_PARAM(Name, Param) hod::ProfilerBeginEvent(Name, Param);
#define PROFILER_END_EVENT() hod::ProfilerEndEvent();
