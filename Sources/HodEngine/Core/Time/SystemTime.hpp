#pragma once
#include "HodEngine/Core/Export.hpp"

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
#include <time.h>
#endif

namespace hod
{
	class HOD_CORE_API SystemTime
	{
	public:
#if defined(PLATFORM_WINDOWS)
		using TimeStamp = __int64;
		static constexpr TimeStamp INVALID_TIMESTAMP = 0;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
		using TimeStamp = timespec;
		static constexpr TimeStamp INVALID_TIMESTAMP = { 0, 0 };
#endif
	public:

		static TimeStamp Now();

		static double ToSeconds(const TimeStamp& timeStamp);
		static double ToMilliseconds(const TimeStamp& timeStamp);

		static double ElapsedTimeInSeconds(const TimeStamp& startTimeStamp, const TimeStamp& endTimeStamp);
		static double ElapsedTimeInMilliseconds(const TimeStamp& startTimeStamp, const TimeStamp& endTimeStamp);

	public:

		SystemTime() = delete;

	private:
#if defined(PLATFORM_WINDOWS)
		static const double	_reverseFrequency;
		static const TimeStamp	_startTimeStamp;
#endif
	};
}
