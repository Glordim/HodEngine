#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Frame/FrameStep.hpp"

#include "HodEngine/Core/Job/JobScheduler.hpp"
#include "HodEngine/Core/Job/Job.hpp"

#include <assert.h>
#include <algorithm>

namespace hod
{
	/// @brief 
	/// @param job 
	void FrameStep::InsertJob(Job* job)
	{
		assert(std::find(_jobs.begin(), _jobs.end(), job) == _jobs.end());

		_jobs.push_back(job);
	}

	/// @brief 
	/// @param job 
	void FrameStep::RemoveJob(Job* job)
	{
		std::vector<Job*>::iterator jobIt = std::find(_jobs.begin(), _jobs.end(), job);

		assert(jobIt != _jobs.end());

		_jobs.erase(jobIt);
	}

	/// @brief 
	void FrameStep::Enqueue()
	{
		JobScheduler* jobScheduler = JobScheduler::GetInstance();

		uint32_t jobCount = (uint32_t)_jobs.size();
		for (uint32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex)
		{
			Job* job = _jobs[jobIndex];

			jobScheduler->Push(job);
		}
	}

	/// @brief 
	void FrameStep::Wait()
	{
		uint32_t jobCount = (uint32_t)_jobs.size();
		for (uint32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex)
		{
			Job* job = _jobs[jobIndex];
			job->Wait();
		}
	}
}
