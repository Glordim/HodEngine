#pragma once

#include "HodEngine/Core/Src/LockFreeQueue.h"
#include "HodEngine/Core/Src/Job/Thread.h"

#include <queue>

namespace HOD
{
	class Job;

	/// @brief 
	class JobQueue
	{
	public:

		enum Queue
		{
			FramedHighPriority = 0,
			FramedNormalPriority,
			FramedLowPriority,

			UnframedHighPriority,
			UnframedNormalPriority,
			UnframedLowPriority,

			Count
		};

	public:

				JobQueue();
				~JobQueue();

		void	Init(Queue queue);
		void	Enqueue(Job* job);
		Job*	PopNextJob();

	private:

		struct WorkerThread
		{
			Thread				_thread;
			bool				_shouldExit = false;
			std::queue<Job*>	_dedicatedJobQueue;
			JobQueue*			_jobQueue = nullptr;
		};

	private:

		static int WorkerThreadFunction(void* parameters);

	private:

		uint32_t					_workerThreadCount = 0;
		WorkerThread*				_workerThreads = nullptr;
		LockFreeQueue<Job*, 4096>	_jobs;
	};
}
