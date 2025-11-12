#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Time/SystemTime.hpp"

#include <win32/misc.h>

namespace hod
{
	/// @brief
	/// @return
	SystemTime::TimeStamp ComputeProcessTimeCounterFrequency()
	{
		SystemTime::TimeStamp performanceTimeStampFrequency = 0;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&performanceTimeStampFrequency));

		return performanceTimeStampFrequency;
	}

	/// @brief
	/// @return
	SystemTime::TimeStamp ComputeStartTimeStamp()
	{
		SystemTime::TimeStamp timeStampStartTime = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeStampStartTime));

		return timeStampStartTime;
	}

	const double                SystemTime::_reverseFrequency = 1.0 / ComputeProcessTimeCounterFrequency();
	const SystemTime::TimeStamp SystemTime::_startTimeStamp = ComputeStartTimeStamp();

	/// @brief
	/// @return
	SystemTime::TimeStamp SystemTime::Now()
	{
		SystemTime::TimeStamp performanceTimeStamp = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&performanceTimeStamp));

		return performanceTimeStamp;
	}

	/// @brief
	/// @param timeStamp
	/// @return
	double SystemTime::ToSeconds(const TimeStamp& timeStamp)
	{
		return (timeStamp - _startTimeStamp) * _reverseFrequency;
	}

	/// @brief
	/// @param TimeStamp
	/// @return
	double SystemTime::ToMilliseconds(const TimeStamp& TimeStamp)
	{
		return (TimeStamp - _startTimeStamp) * _reverseFrequency * 1000.0;
	}

	/// @brief
	/// @param Start
	/// @param End
	/// @return
	double SystemTime::ElapsedTimeInSeconds(const TimeStamp& Start, const TimeStamp& End)
	{
		return (static_cast<double>(End) - static_cast<double>(Start)) * _reverseFrequency;
	}

	/// @brief
	/// @param Start
	/// @param End
	/// @return
	double SystemTime::ElapsedTimeInMilliseconds(const TimeStamp& Start, const TimeStamp& End)
	{
		return (static_cast<double>(End) - static_cast<double>(Start)) * _reverseFrequency * 1000.0;
	}
}
