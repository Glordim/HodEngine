#include "JobQueue.h"

#include "HodEngine/Core/Src/SystemInfo.h"
#include "HodEngine/Core/Src/Job/Job.h"

#include <string>

namespace HOD
{
	/// @brief 
	const char* jobQueueNames[JobQueue::Queue::Count] = {
		"FrameLowPriority",
		"FrameNormalPriority",
		"FrameHighPriority",
		"UnframedLowPriority",
		"UnframedNormalPriority",
		"UnframedHighPriority",
	};

	/// @brief 
	Thread::Priority jobQueuePriority[JobQueue::Queue::Count] = {
		Thread::Priority::Low,
		Thread::Priority::Normal,
		Thread::Priority::High,
		Thread::Priority::Low,
		Thread::Priority::Normal,
		Thread::Priority::High,
	};

	/// @brief 
	/// @param  
	/// @return 
	int JobQueue::WorkerThreadFunction(void* parameters)
	{
		WorkerThread* workerThread = static_cast<WorkerThread*>(parameters);

		Job* job = nullptr;

		while (workerThread->_shouldExit == false)
		{
			while (workerThread->_dedicatedJobQueue.empty() == false)
			{
				job = workerThread->_dedicatedJobQueue.front();
				workerThread->_dedicatedJobQueue.pop();

				job->Execute();
			}

			job = workerThread->_jobQueue->PopNextJob();
			while (job != nullptr)
			{
				job->Execute();
				job = workerThread->_jobQueue->PopNextJob();
			}

			workerThread->_thread.Sleep(1000);
		}

		return 0;
	}

	/// @brief 
	JobQueue::JobQueue()
	{

	}

	/// @brief 
	JobQueue::~JobQueue()
	{
		delete[] _workerThreads;
	}

	/// @brief 
	void JobQueue::Init(Queue queue)
	{
		uint32_t count = SystemInfo::GetLogicalCoreCount();
		_workerThreads = new WorkerThread[count];

		for (uint32_t index = 0; index < count; ++index)
		{
			std::string workerName(jobQueueNames[queue]);
			workerName += " Worker ";

			std::string countLabel = std::to_string(count + 1);

			std::string workerIndexLabel = std::to_string(index + 1);
			for (size_t digit = workerIndexLabel.size(); digit < countLabel.size(); ++digit)
			{
				workerIndexLabel.insert(0, "0");
			}
			workerName += workerIndexLabel;

			_workerThreads[index]._jobQueue = this;
			_workerThreads[index]._thread.Start(&WorkerThreadFunction, &_workerThreads[index], jobQueuePriority[queue], workerName.c_str());
		}
	}

	/// @brief 
	void JobQueue::Enqueue(Job* job)
	{
		if (job->GetThreadId() == Thread::InvalidId)
		{
			while (_jobs.Push(job) == false)
			{
				// Try while Queue works
			}

			job->SetQueued();
		}
		else
		{
			for (uint32_t workerThreadIndex = 0; workerThreadIndex < _workerThreadCount; ++workerThreadIndex)
			{
				_workerThreads[workerThreadIndex]._dedicatedJobQueue.push(job);
				job->SetQueued();
				break;
			}
		}
	}

	/// @brief 
	/// @return 
	Job* JobQueue::PopNextJob()
	{
		Job* job = nullptr;
		_jobs.Pop(job);
		return job;
	}
}
