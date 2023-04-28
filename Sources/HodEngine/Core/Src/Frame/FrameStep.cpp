#include "Core/Frame/FrameStep.h"

#include "Core/Job/JobSqueduler.h"
#include "Core/Job/Job.h"

#include <assert.h>

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

		assert(jobIt == _jobs.end());

		_jobs.erase(jobIt);
	}

	/// @brief 
	void FrameStep::Enqueue()
	{
		JobSqueduler* jobSqueduler = JobSqueduler::GetInstance();

		uint32_t jobCount = (uint32_t)_jobs.size();
		for (uint32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex)
		{
			Job* job = _jobs[jobIndex];

			jobSqueduler->Push(job);
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
