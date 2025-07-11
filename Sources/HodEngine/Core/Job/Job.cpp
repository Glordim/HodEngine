#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Job/Job.hpp"

#include <cassert>

namespace hod
{
	/// @brief 
	/// @param queue 
	/// @param deleteAfterCompletion 
	Job::Job(JobQueue::Queue queue, bool deleteAfterCompletion, Thread::Id threadId)
		: _threadId(threadId)
		, _queue(queue)
		, _isDeleteAfterCompletion(deleteAfterCompletion)
	{

	}

	/// @brief 
	/// @return 
	Thread::Id Job::GetThreadId() const
	{
		return _threadId;
	}

	/// @brief 
	/// @return 
	JobQueue::Queue Job::GetQueue() const
	{
		return _queue;
	}

	/// @brief 
	/// @return 
	bool Job::IsDeleteAfterCompletion() const
	{
		return _isDeleteAfterCompletion;
	}

	/// @brief 
	void Job::Prepare()
	{
		_state = 0;
		_waitFlag.clear();
	}

	/// @brief 
	void Job::SetQueued()
	{
		_state |= State::Queued;
	}

	/// @brief 
	void Job::Execute()
	{
		while ((_state & State::Queued) == 0);
		_state &= ~State::Queued;
		if ((_state & State::Canceled) == 0)
		{
			_state |= State::Executing;

			Execution();

			_state &= ~State::Executing;
		}
		else
		{
			_state &= ~State::Canceled;
		}
		_state |= State::Completed;
		_waitFlag.test_and_set();
		_waitFlag.notify_all();

		if (_isDeleteAfterCompletion == true)
		{
			DefaultAllocator::GetInstance().Delete(this);
		}
	}

	/// @brief 
	bool Job::Cancel()
	{
		_state |= State::Canceled;
		return (_state & (State::Queued | State::Executing)) != 0;
	}

	/// @brief 
	void Job::Wait()
	{
		if ((_state & State::Completed) == 0)
		{
			_waitFlag.wait(false);
		}
	}
}
