#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/GameSystems/Job/JobScheduler.hpp"

#include "HodEngine/GameSystems/Job/Job.hpp"

namespace hod::inline gamesystems
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
