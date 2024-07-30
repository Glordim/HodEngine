#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Frame/FrameSequencer.hpp"

#include <stdint.h>

namespace hod
{
	/// @brief 
	FrameSequencer::FrameSequencer()
	{

	}

	/// @brief 
	FrameSequencer::~FrameSequencer()
	{

	}

	/// @brief 
	/// @param job 
	/// @param step 
	void FrameSequencer::InsertJob(Job* job, Step step)
	{
		_steps[static_cast<uint8_t>(step)].InsertJob(job);
	}

	/// @brief 
	/// @param job 
	/// @param step 
	void FrameSequencer::RemoveJob(Job* job, Step step)
	{
		_steps[static_cast<uint8_t>(step)].RemoveJob(job);
	}

	/// @brief 
	void FrameSequencer::EnqueueAndWaitJobs()
	{
		for (uint8_t stepIndex = 0; stepIndex < static_cast<uint8_t>(Step::Count); ++stepIndex)
		{
			_steps[stepIndex].Enqueue();
			_steps[stepIndex].Wait();
		}
	}
}
