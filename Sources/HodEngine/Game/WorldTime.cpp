#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/WorldTime.hpp"

#include "HodEngine/Core/Math.hpp"

namespace hod::inline game
{
	/// @brief
	/// @param realDeltaTime
	void WorldTime::Advance(float realDeltaTime)
	{
		_unscaledDeltaTime = Clamp(realDeltaTime, 0.0f, _maxDeltaTime);
		_deltaTime = _unscaledDeltaTime * _timeScale;

		_unscaledElapsed += _unscaledDeltaTime;
		_elapsed += _deltaTime;
		++_frameCount;

		_accumulatedFixedTime += _deltaTime;
	}

	/// @brief
	/// @return
	bool WorldTime::ConsumeFixedStep()
	{
		if (_accumulatedFixedTime < _fixedTimestep)
		{
			return false;
		}
		_accumulatedFixedTime -= _fixedTimestep;
		return true;
	}

	/// @brief
	/// @return
	float WorldTime::GetDeltaTime() const
	{
		return _deltaTime;
	}

	/// @brief
	/// @return
	float WorldTime::GetUnscaledDeltaTime() const
	{
		return _unscaledDeltaTime;
	}

	/// @brief
	/// @return
	double WorldTime::GetElapsed() const
	{
		return _elapsed;
	}

	/// @brief
	/// @return
	double WorldTime::GetUnscaledElapsed() const
	{
		return _unscaledElapsed;
	}

	/// @brief
	/// @return
	uint64_t WorldTime::GetFrameCount() const
	{
		return _frameCount;
	}

	/// @brief
	/// @param timeScale
	void WorldTime::SetTimeScale(float timeScale)
	{
		_timeScale = Max(timeScale, 0.0f);
	}

	/// @brief
	/// @return
	float WorldTime::GetTimeScale() const
	{
		return _timeScale;
	}

	/// @brief
	/// @param maxDeltaTime
	void WorldTime::SetMaxDeltaTime(float maxDeltaTime)
	{
		_maxDeltaTime = Max(maxDeltaTime, 0.0f);
	}

	/// @brief
	/// @return
	float WorldTime::GetMaxDeltaTime() const
	{
		return _maxDeltaTime;
	}

	/// @brief
	/// @return
	float WorldTime::GetFixedTimestep() const
	{
		return _fixedTimestep;
	}
}
