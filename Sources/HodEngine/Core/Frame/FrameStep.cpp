#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Frame/FrameStep.hpp"

#include "HodEngine/Core/Job/Job.hpp"
#include "HodEngine/Core/Job/JobScheduler.hpp"

#include <cassert>

namespace hod
{
	/// @brief
	/// @param job
	void FrameStep::InsertJob(Job* job)
	{
		assert(std::find(_jobs.Begin(), _jobs.End(), job) == _jobs.End());

		_jobs.push_back(job);
	}

	/// @brief
	/// @param job
	void FrameStep::RemoveJob(Job* job)
	{
		Vector<Job*>::Iterator jobIt = std::find(_jobs.Begin(), _jobs.End(), job);

		assert(jobIt != _jobs.End());

		_jobs.Erase(jobIt);
	}

	/// @brief
	void FrameStep::Enqueue()
	{
		JobScheduler* jobScheduler = JobScheduler::GetInstance();

		uint32_t jobCount = (uint32_t)_jobs.Size();
		for (uint32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex)
		{
			Job* job = _jobs[jobIndex];
			jobScheduler->Push(job);
		}
	}

	/// @brief
	void FrameStep::Wait()
	{
		uint32_t jobCount = (uint32_t)_jobs.Size();
		for (uint32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex)
		{
			Job* job = _jobs[jobIndex];
			job->Wait();
		}
	}
}
