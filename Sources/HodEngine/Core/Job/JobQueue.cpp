#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Job/JobQueue.hpp"
#include "HodEngine/Core/Job/Job.hpp"

#include "HodEngine/Core/SystemInfo.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/String.hpp"
#include <atomic>

namespace hod
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
			while (workerThread->_dedicatedJobQueue.Dequeue(job) == true)
			{
				job->Execute();
			}

			job = workerThread->_jobQueue->PopNextJob();
			while (job != nullptr)
			{
				job->Execute();
				job = workerThread->_jobQueue->PopNextJob();
			}

			workerThread->_wakeUpFlag.wait(false);
			workerThread->_wakeUpFlag.clear();
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
		for (uint32_t workerThreadIndex = 0; workerThreadIndex < _workerThreadCount; ++workerThreadIndex)
		{
			_workerThreads[workerThreadIndex]._shouldExit = true;
			if (_workerThreads[workerThreadIndex]._wakeUpFlag.test_and_set() == false)
			{
				_workerThreads[workerThreadIndex]._wakeUpFlag.notify_all();
				break;
			}
		}
		for (uint32_t workerThreadIndex = 0; workerThreadIndex < _workerThreadCount; ++workerThreadIndex)
		{
			_workerThreads[workerThreadIndex]._thread.Join();
		}

		delete[] _workerThreads;
	}

	/// @brief 
	void JobQueue::Init(Queue queue)
	{
		uint32_t count = SystemInfo::GetLogicalCoreCount();
		count = 1; // TODO REMOVE
		_workerThreadCount = count;
		_workerThreads = new WorkerThread[count];

		for (uint32_t index = 0; index < count; ++index)
		{
			String workerName(jobQueueNames[queue]);
			workerName += " Worker ";

			String countLabel = std::to_string(count + 1);

			String workerIndexLabel = std::to_string(index + 1);
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
			job->Prepare();
			while (_jobs.Enqueue(job) == false)
			{
				OUTPUT_ERROR("JobQueue Full !");
			}
			job->SetQueued();
			for (uint32_t workerThreadIndex = 0; workerThreadIndex < _workerThreadCount; ++workerThreadIndex)
			{
				if (_workerThreads[workerThreadIndex]._wakeUpFlag.test_and_set() == false)
				{
					_workerThreads[workerThreadIndex]._wakeUpFlag.notify_all();
					break;
				}
			}
		}
		else
		{
			for (uint32_t workerThreadIndex = 0; workerThreadIndex < _workerThreadCount; ++workerThreadIndex)
			{
				if (_workerThreads[workerThreadIndex]._thread.GetId() == job->GetThreadId())
				{
					job->Prepare();
					while (_workerThreads[workerThreadIndex]._dedicatedJobQueue.Enqueue(job) == false)
					{
						OUTPUT_ERROR("JobQueue Full !");
					}
					job->SetQueued();
					_workerThreads[workerThreadIndex]._wakeUpFlag.test_and_set();
					_workerThreads[workerThreadIndex]._wakeUpFlag.notify_all();
					break;
				}
			}
		}
	}

	/// @brief 
	/// @return 
	Job* JobQueue::PopNextJob()
	{
		Job* job = nullptr;
		_jobs.Dequeue(job);
		return job;
	}
}
