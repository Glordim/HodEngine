#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Job/JobScheduler.hpp"

#include "HodEngine/Core/Job/Job.hpp"

namespace hod
{
	/// @brief 
	JobScheduler::JobScheduler()
	{
		for (uint32_t index = 0; index < static_cast<uint32_t>(JobQueue::Queue::COUNT); ++index)
		{
			_queues[index].Init(static_cast<JobQueue::Queue>(index));
		}
	}

	/// @brief 
	/// @param job 
	void JobScheduler::Push(Job* job)
	{
		_queues[(uint32_t)job->GetQueue()].Enqueue(job);
	}
}
