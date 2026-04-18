#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::inline gamesystems
{
	class Job;

	/// @brief
	class HOD_GAME_SYSTEMS_API FrameStep
	{
	public:
		void InsertJob(Job* job);
		void RemoveJob(Job* job);

		void Enqueue();
		void Wait();

	private:
		Vector<Job*> _jobs;
	};
}
