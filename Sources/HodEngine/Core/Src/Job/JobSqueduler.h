#pragma once

#include "HodEngine/Core/Src/Singleton.h"
#include "HodEngine/Core/Src/Job/JobQueue.h"

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
