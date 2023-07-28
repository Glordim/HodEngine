#pragma once

#include <vector>

#include "HodEngine/Core/Job/JobQueue.h"

namespace hod
{
	/// @brief 
	class Job
	{
	public:

		enum State
		{
			Queued		= 1 << 0,
			Executing	= 1 << 1,
			Completed	= 1 << 2,
			Canceled	= 1 << 3,
		};

	public:

						Job(JobQueue::Queue queue, bool deleteAfterCompletion, Thread::Id threadId = Thread::InvalidId);
						Job(const Job&) = delete;
						Job(Job&&) = delete;
		virtual			~Job() = default;

		Job&			operator = (const Job&) = delete;
		Job&			operator = (Job&&) = delete;

	public:

		void			SetQueued();

		void			Execute();
		bool			Cancel();
		void			Wait();

		Thread::Id		GetThreadId() const;
		JobQueue::Queue	GetQueue() const;
		bool			IsDeleteAfterCompletion() const;

	protected:

		virtual void	Execution() = 0;

	private:

		uint8_t			_state = 0;

		Thread::Id		_threadId;
		JobQueue::Queue	_queue;
		bool			_isDeleteAfterCompletion;
	};
}
