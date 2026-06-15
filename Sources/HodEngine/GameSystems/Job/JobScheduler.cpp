#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/GameSystems/Job/JobScheduler.hpp"

#include "HodEngine/GameSystems/Job/Job.hpp"

#include <TaskScheduler.h>

namespace hod::inline gamesystems
{
	/// @brief
	JobScheduler::JobScheduler()
	{
		_frameScheduler = DefaultAllocator::GetInstance().New<enki::TaskScheduler>();
		_backgroundScheduler = DefaultAllocator::GetInstance().New<enki::TaskScheduler>();

		/*
		for (uint32_t index = 0; index < static_cast<uint32_t>(JobQueue::Queue::COUNT); ++index)
		{
			_queues[index].Init(static_cast<JobQueue::Queue>(index));
		}
		*/
	}

	JobScheduler::~JobScheduler()
	{
		DefaultAllocator::GetInstance().Delete(_frameScheduler);
		DefaultAllocator::GetInstance().Delete(_backgroundScheduler);
	}

	bool JobScheduler::Init()
	{
		_frameScheduler->Initialize();
		_backgroundScheduler->Initialize();
		return true;
	}

	/// @brief
	/// @param job
	void JobScheduler::Push(Job* job)
	{
		job->_taskScheduler = _frameScheduler;
		_frameScheduler->AddTaskSetToPipe(job->_taskSet);
	}

	void JobScheduler::PushBackground(Job* job)
	{
		job->_taskScheduler = _backgroundScheduler;
		_backgroundScheduler->AddTaskSetToPipe(job->_taskSet);
	}

	void JobScheduler::Wait()
	{
		_frameScheduler->WaitforAll();
	}

	void JobScheduler::WaitBackground()
	{
		_backgroundScheduler->WaitforAll();
	}
}
