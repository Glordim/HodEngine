#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Core/Job/JobQueue.hpp"

#include <stdint.h>

namespace hod
{
	class Job;

	/// @brief 
	class HOD_CORE_API JobScheduler : public Singleton<JobScheduler>
	{
		friend class Singleton<JobScheduler>;
		friend class Allocator;

	public:

		void		Push(Job* job);

	protected:

					JobScheduler();

	private:

		JobQueue	_queues[JobQueue::Queue::Count];
	};
}
