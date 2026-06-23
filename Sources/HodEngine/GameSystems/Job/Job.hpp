#pragma once
#include "HodEngine/GameSystems/Export.hpp"

namespace enki
{
	class TaskSet;
	class TaskScheduler;
}

namespace hod::inline gamesystems
{
	/// @brief
	class HOD_GAME_SYSTEMS_API Job
	{
		friend class JobScheduler;

	public:
		Job();
		Job(const Job&) = delete;
		Job(Job&&) = delete;
		virtual ~Job();

		Job& operator=(const Job&) = delete;
		Job& operator=(Job&&) = delete;

		void SetAutoDelete(bool autoDelete) { _autoDelete = autoDelete; }

	public:
		void Wait();

	protected:
		virtual void Execution() = 0;

	private:
		enki::TaskSet* _taskSet = nullptr;
		enki::TaskScheduler* _taskScheduler = nullptr;
		bool _autoDelete = false;

		std::atomic<Job*> _nextCompleted = nullptr;
	};
}
