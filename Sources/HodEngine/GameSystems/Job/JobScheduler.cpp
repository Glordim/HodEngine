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
		if (job->_pinnedThreadId == 0xFFFFFFFF)
		{
			_frameScheduler->AddTaskSetToPipe(static_cast<enki::ITaskSet*>(job->_completable));
		}
		else
		{
			_frameScheduler->AddPinnedTask(static_cast<enki::IPinnedTask*>(job->_completable));
		}
	}

	void JobScheduler::PushBackground(Job* job)
	{
		job->_taskScheduler = _backgroundScheduler;
		if (job->_pinnedThreadId == 0xFFFFFFFF)
		{
			_backgroundScheduler->AddTaskSetToPipe(static_cast<enki::ITaskSet*>(job->_completable));
		}
		else
		{
			_backgroundScheduler->AddPinnedTask(static_cast<enki::IPinnedTask*>(job->_completable));
		}
	}

	void JobScheduler::Wait()
	{
		_frameScheduler->WaitforAll();
	}

	void JobScheduler::WaitBackground()
	{
		_backgroundScheduler->WaitforAll();
	}

	void JobScheduler::MarkForCleanup(Job* job)
	{
		Job* head = _completedHead.load(std::memory_order_relaxed);
		do
		{
			job->_nextCompleted = head;
		} while (!_completedHead.compare_exchange_weak(head, job, std::memory_order_release, std::memory_order_relaxed));
	}

	void JobScheduler::CleanupCompleted()
	{
		Job* job = _completedHead.exchange(nullptr, std::memory_order_acquire);
		while (job)
		{
			Job* next = job->_nextCompleted;
			DefaultAllocator::GetInstance().Delete(job);
			job = next;
		}
	}
}
