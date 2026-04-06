#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include "HodEngine/GameSystems/Job/JobQueue.hpp"
#include "HodEngine/Core/Singleton.hpp"

#include "HodEngine/Core/Reflection/EnumTrait.hpp"

#include <stdint.h>

namespace hod
{
	class Job;

	/// @brief
	class HOD_GAME_SYSTEMS_API JobScheduler : public Singleton<JobScheduler>
	{
		friend class Singleton<JobScheduler>;
		friend class Allocator;

	public:
		void Push(Job* job);

	protected:
		JobScheduler();

	private:
		JobQueue _queues[static_cast<uint32_t>(JobQueue::Queue::COUNT)];
	};
}
