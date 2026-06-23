#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/GameSystems/Job/Job.hpp"

#include "HodEngine/GameSystems/Job/JobScheduler.hpp"
#include <TaskScheduler.h>
#include <cassert>

namespace hod::inline gamesystems
{
	/// @brief
	Job::Job()
	{
		_taskSet = DefaultAllocator::GetInstance().New<enki::TaskSet>([&](enki::TaskSetPartition /*range*/, uint32_t /*threadnum*/)
		{
			Execution();

			if (_autoDelete)
			{
				JobScheduler::GetInstance()->MarkForCleanup(this);
			}
		});
	}

	Job::~Job()
	{
		DefaultAllocator::GetInstance().Delete(_taskSet);
	}

	/// @brief
	void Job::Wait()
	{
		_taskScheduler->WaitforTask(_taskSet);
	}
}
