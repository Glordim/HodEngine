#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/GameSystems/Job/Job.hpp"

#include "HodEngine/GameSystems/Job/JobScheduler.hpp"
#include <cassert>
#include <cstdint>
#include <TaskScheduler.h>


namespace hod::inline gamesystems
{
	/// @brief
	Job::Job(uint32_t threadId)
	{
		_pinnedThreadId = threadId;
		if (threadId == enki::NO_THREAD_NUM)
		{
			_completable = DefaultAllocator::GetInstance().New<enki::TaskSet>(
				[&](enki::TaskSetPartition /*range*/, uint32_t /*threadnum*/)
				{
					Execution();

					if (_autoDelete)
					{
						JobScheduler::GetInstance()->MarkForCleanup(this);
					}
				});
		}
		else
		{
			_completable = DefaultAllocator::GetInstance().New<enki::LambdaPinnedTask>(threadId,
			                                                                           [&]()
			                                                                           {
																						   Execution();

																						   if (_autoDelete)
																						   {
																							   JobScheduler::GetInstance()->MarkForCleanup(this);
																						   }
																					   });
		}
	}

	Job::~Job()
	{
		DefaultAllocator::GetInstance().Delete(_completable);
	}

	/// @brief
	void Job::Wait()
	{
		_taskScheduler->WaitforTask(_completable);
	}
}
