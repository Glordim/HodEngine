#pragma once
#include "HodEngine/Game/Export.hpp"

#include <cstdint>

namespace hod::inline game
{
	/// @brief The time of a world (World::GetTime): its frame's delta, its elapsed time, its scale and its fixed steps.
	/// Scaled time is the gameplay's (OnUpdate's deltaTime, the fixed steps), unscaled time the real one (a pause menu's UI).
	/// A time scale of 0 pauses the gameplay: Update still called with a delta of 0, no fixed step anymore (physics stopped)
	class HOD_GAME_API WorldTime
	{
	public:
		/// @brief A new frame: the real time elapsed since the previous one, capped (see SetMaxDeltaTime), then scaled. Called by World::Update
		void		Advance(float realDeltaTime);
		/// @brief Called in a loop after Advance, a fixed step each time it returns true (as many as the scaled time elapsed holds)
		bool		ConsumeFixedStep();

		/// @return The frame's delta in seconds, scaled (OnUpdate's deltaTime)
		float		GetDeltaTime() const;
		/// @return The frame's delta in seconds, real (capped, not scaled)
		float		GetUnscaledDeltaTime() const;

		/// @return The seconds elapsed since the world started running, scaled (frozen while the time scale is 0)
		double		GetElapsed() const;
		/// @return The seconds elapsed since the world started running, real (capped deltas, not scaled)
		double		GetUnscaledElapsed() const;

		/// @return The frames run since the world started running
		uint64_t	GetFrameCount() const;

		/// @brief 1 by default, 0 pauses the gameplay, 0.5 a slow motion. Negative: 0
		void		SetTimeScale(float timeScale);
		float		GetTimeScale() const;

		/// @brief A frame's real delta capped to it (0.1s by default): after a hitch or a breakpoint, the world doesn't jump nor run a burst of
		/// fixed steps to catch up
		void		SetMaxDeltaTime(float maxDeltaTime);
		float		GetMaxDeltaTime() const;

		/// @return The duration of a fixed step in seconds, scaled time (OnFixedUpdate's)
		float		GetFixedTimestep() const;

	private:
		float		_deltaTime = 0.0f;
		float		_unscaledDeltaTime = 0.0f;
		double		_elapsed = 0.0;
		double		_unscaledElapsed = 0.0;
		uint64_t	_frameCount = 0;

		float		_timeScale = 1.0f;
		float		_maxDeltaTime = 0.1f;

		float		_fixedTimestep = 1.0f / 120.0f;
		float		_accumulatedFixedTime = 0.0f; // Scaled time not run in fixed steps yet
	};
}
