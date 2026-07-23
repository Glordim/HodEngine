#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include <HodEngine/Core/Thread/Thread.hpp>

namespace enki
{
	class ICompletable;
	class TaskScheduler;
}

namespace hod::inline gamesystems
{
	/// @brief
	class HOD_GAME_SYSTEMS_API Job
	{
		friend class JobScheduler;

	public:
		Job(uint32_t threadId = 0xFFFFFFFF);
		Job(const Job&) = delete;
		Job(Job&&) = delete;
		virtual ~Job();

		Job& operator=(const Job&) = delete;
		Job& operator=(Job&&) = delete;

		void SetAutoDelete(bool autoDelete)
		{
			_autoDelete = autoDelete;
		}

	public:
		void Wait();

	protected:
		virtual void Execution() = 0;

	private:
		enki::ICompletable*  _completable = nullptr;
		enki::TaskScheduler* _taskScheduler = nullptr;
		bool                 _autoDelete = false;

		std::atomic<Job*> _nextCompleted = nullptr;
		uint32_t          _pinnedThreadId = 0;
	};
}
