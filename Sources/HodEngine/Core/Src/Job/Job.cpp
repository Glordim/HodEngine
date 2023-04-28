#include "Core/Job/Job.h"

namespace hod
{
	/// @brief 
	/// @param queue 
	/// @param deleteAfterCompletion 
	Job::Job(JobQueue::Queue queue, bool deleteAfterCompletion, Thread::Id threadId)
		: _queue(queue)
		, _isDeleteAfterCompletion(deleteAfterCompletion)
		, _threadId(threadId)
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
	void Job::SetQueued()
	{
		_state = State::Queued;
	}

	/// @brief 
	void Job::Execute()
	{
		_state &= ~State::Queued;
		if ((_state & State::Canceled) == 0)
		{
			_state |= State::Executing;

			Execution();

			_state &= ~State::Executing;
		}
		_state |= State::Completed;

		if (_isDeleteAfterCompletion == true)
		{
			delete this;
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
		if (_state & (State::Queued | State::Executing))
		{
			while ((_state & State::Completed) == 0);
		}
	}
}
