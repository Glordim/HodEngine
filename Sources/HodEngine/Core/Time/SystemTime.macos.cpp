#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Time/SystemTime.hpp"

#include <cassert>

namespace hod::inline core
{
	const SystemTime::TimeStamp SystemTime::_startTimeStamp = SystemTime::Now();

	/// @brief
	/// @return
	SystemTime::TimeStamp SystemTime::Now()
	{
		timespec ts;
		[[maybe_unused]] int      result = clock_gettime(CLOCK_REALTIME, &ts);
		assert(result == 0);

		return ts;
	}

	/// @brief
	/// @param timeStamp
	/// @return
	double SystemTime::ToSeconds(const TimeStamp& timeStamp)
	{
		double second = static_cast<double>(timeStamp.tv_sec - _startTimeStamp.tv_sec);
		second += static_cast<double>(timeStamp.tv_nsec - _startTimeStamp.tv_nsec) * 0.000000001;
		return second;
	}

	/// @brief
	/// @param timeStamp
	/// @return
	double SystemTime::ToMilliseconds(const TimeStamp& timeStamp)
	{
		double second = static_cast<double>(timeStamp.tv_sec - _startTimeStamp.tv_sec) * 1000.0;
		second += static_cast<double>(timeStamp.tv_nsec - _startTimeStamp.tv_nsec) * 0.000001;
		return second;
	}

	/// @brief
	/// @param start
	/// @param end
	/// @return
	double SystemTime::ElapsedTimeInSeconds(const TimeStamp& start, const TimeStamp& end)
	{
		return static_cast<double>(end.tv_sec - start.tv_sec) + static_cast<double>(end.tv_nsec - start.tv_nsec) * 0.000000001;
	}

	/// @brief
	/// @param start
	/// @param end
	/// @return
	double SystemTime::ElapsedTimeInMilliseconds(const TimeStamp& start, const TimeStamp& end)
	{
		return static_cast<double>(end.tv_sec - start.tv_sec) * 1000.0 + static_cast<double>(end.tv_nsec - start.tv_nsec) * 0.000001;
	}
}
