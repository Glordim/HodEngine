#pragma once

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Core/Job/JobQueue.hpp"

#include <stdint.h>

namespace hod
{
	class Job;

	/// @brief 
	class JobScheduler : public Singleton<JobScheduler>
	{
		friend class Singleton<JobScheduler>;

	public:

		void		Push(Job* job);

	protected:

					JobScheduler();

	private:

		JobQueue	_queues[JobQueue::Queue::Count];
	};
}