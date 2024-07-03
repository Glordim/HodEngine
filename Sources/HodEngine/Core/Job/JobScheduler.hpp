#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Core/Job/JobQueue.hpp"

#include <stdint.h>

namespace hod
{
	class Job;

	/// @brief 
	class HOD_API JobScheduler : public Singleton<JobScheduler>
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
