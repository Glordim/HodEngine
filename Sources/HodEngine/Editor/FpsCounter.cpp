#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FpsCounter.hpp"

namespace hod::editor
{
	/// @brief
	/// @param sampleCount
	FpsCounter::FpsCounter(uint32_t sampleCount)
	: _sampleCount(sampleCount)
	, _frameTimes(sampleCount)
	, _currentIndex(0)
	, _frameCount(0)
	{
		_lastTime = SystemTime::Now();
		_frameTimes.Resize(sampleCount, 0.0f);
	}

	/// @brief
	void FpsCounter::OnFrame()
	{
		SystemTime::TimeStamp now = SystemTime::Now();
		float                 frameDuration = (float)SystemTime::ElapsedTimeInMilliseconds(_lastTime, now);
		_lastTime = now;

		if (_frameCount == _sampleCount)
		{
			_totalDuration -= _frameTimes[_currentIndex];
		}

		_frameTimes[_currentIndex] = frameDuration;
		_totalDuration += frameDuration;
		_currentIndex = (_currentIndex + 1) % _sampleCount;
		if (_frameCount < _sampleCount)
		{
			++_frameCount;
		}
	}

	/// @brief
	/// @return
	float FpsCounter::GetFPS() const
	{
		if (_frameCount == 0 || _totalDuration == 0.0f)
		{
			return 0.0f;
		}

		return _frameCount / (_totalDuration * 0.001f);
	}
}
