#pragma once
#include "HodEngine/Editor/Export.hpp"
#include <HodEngine/Core/Time/SystemTime.hpp>

#include "HodEngine/Core/Vector.hpp"

namespace hod::editor
{
	/// @brief 
	class FpsCounter
	{
	public:

		FpsCounter(uint32_t sampleCount = 64);

		void OnFrame();
		float GetFPS() const;

	private:

		uint32_t 				_sampleCount;
		SystemTime::TimeStamp	_lastTime;
		Vector<float>		_frameTimes;
		uint32_t				_currentIndex;
		uint32_t				_frameCount;
		float					_totalDuration = 0.0f;
	};
}
