#pragma once

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Core/Job/JobQueue.hpp"

#include <stdint.h>

namespace hod
{
	class Job;

	/// @brief 
	class JobSqueduler : public Singleton<JobSqueduler>
	{
		friend class Singleton<JobSqueduler>;

	public:

		void		Push(Job* job);

	protected:

					JobSqueduler();

	private:

		JobQueue	_queues[JobQueue::Queue::Count];
	};
}
