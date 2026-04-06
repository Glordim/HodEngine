#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include "HodEngine/GameSystems/Job/LockFreeQueue.hpp"
#include "HodEngine/Core/Thread/Thread.hpp"

#include <atomic>

namespace hod
{
	class Job;

	/// @brief
	class HOD_GAME_SYSTEMS_API JobQueue
	{
	public:
		enum class Queue
		{
			Framed = 0,
			Unframed,

			COUNT
		};

	public:
		JobQueue();
		~JobQueue();

		void Init(Queue queue);
		void Enqueue(Job* job);
		Job* PopNextJob();

	private:
		struct WorkerThread
		{
			Thread                   _thread;
			bool                     _shouldExit = false;
			LockFreeQueue<Job*, 256> _dedicatedJobQueue;
			JobQueue*                _jobQueue = nullptr;
			std::atomic_flag         _wakeUpFlag;
		};

	private:
		static int WorkerThreadFunction(void* parameters);

	private:
		uint32_t                  _workerThreadCount = 0;
		WorkerThread*             _workerThreads = nullptr;
		LockFreeQueue<Job*, 4096> _jobs;
	};
}
