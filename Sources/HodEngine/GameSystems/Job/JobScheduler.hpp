#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"

#include <stdint.h>

namespace enki
{
	class TaskScheduler;
}

namespace hod::inline gamesystems
{
	class Job;

	/// @brief
	class HOD_GAME_SYSTEMS_API JobScheduler : public Singleton<JobScheduler>
	{
		friend class Singleton<JobScheduler>;
		friend class hod::Allocator;

	public:
		bool Init();

		void Push(Job* job);
		void Wait();
		
		void PushBackground(Job* job);
		void WaitBackground();

		void MarkForCleanup(Job* job);
		void CleanupCompleted();

	protected:
		JobScheduler();
		~JobScheduler();

	private:
		enki::TaskScheduler* _frameScheduler = nullptr;
		enki::TaskScheduler* _backgroundScheduler = nullptr;

		std::atomic<Job*> _completedHead = nullptr;
	};
}
