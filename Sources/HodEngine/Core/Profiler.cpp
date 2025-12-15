
#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Profiler.hpp"

#if defined(HOD_USE_SUPERLIMINAL_PROFILER)
	#include <Superluminal/PerformanceAPI_capi.h>
#endif

namespace hod
{
	ProfilerScopedEvent::ProfilerScopedEvent(const char* name)
	{
		ProfilerBeginEvent(name);
	}

	ProfilerScopedEvent::~ProfilerScopedEvent()
	{
		ProfilerEndEvent();
	}

	void ProfilerBeginEvent(const char* name)
	{
#if defined(HOD_USE_SUPERLIMINAL_PROFILER) && defined(PERFORMANCEAPI_ENABLED)
		PerformanceAPI_BeginEvent(name, nullptr, PERFORMANCEAPI_DEFAULT_COLOR);
#endif
	}

	void ProfilerEndEvent()
	{
#if defined(HOD_USE_SUPERLIMINAL_PROFILER) && defined(PERFORMANCEAPI_ENABLED)
		PerformanceAPI_EndEvent();
#endif
	}
}
