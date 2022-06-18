#include "JobSqueduler.h"

#include "HodEngine/Core/Src/Job/Job.h"

namespace hod
{
	//bSqueduler* JobSqueduler::_instance = nullptr;

	/// @brief 
	JobSqueduler::JobSqueduler()
	{
		for (uint32_t index = 0; index < JobQueue::Queue::Count; ++index)
		{
			_queues[index].Init(static_cast<JobQueue::Queue>(index));
		}
	}

	/// @brief 
	/// @param job 
	void JobSqueduler::Push(Job* job)
	{
		_queues[job->GetQueue()].Enqueue(job);
	}
}
