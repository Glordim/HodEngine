
#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Profiler.hpp"

#if defined(HOD_USE_SUPERLIMINAL_PROFILER)
	#include <Superluminal/PerformanceAPI_capi.h>
#endif

namespace hod
{
	ProfilerScopedEvent::ProfilerScopedEvent(const char* name, const char* param)
	{
		ProfilerBeginEvent(name, param);
	}

	ProfilerScopedEvent::~ProfilerScopedEvent()
	{
		ProfilerEndEvent();
	}

	void ProfilerBeginEvent([[maybe_unused]] const char* name, [[maybe_unused]] const char* param)
	{
#if defined(HOD_USE_SUPERLIMINAL_PROFILER) && defined(PERFORMANCEAPI_ENABLED)
		PerformanceAPI_BeginEvent(name, param, PERFORMANCEAPI_DEFAULT_COLOR);
#endif
	}

	void ProfilerEndEvent()
	{
#if defined(HOD_USE_SUPERLIMINAL_PROFILER) && defined(PERFORMANCEAPI_ENABLED)
		PerformanceAPI_EndEvent();
#endif
	}
}
