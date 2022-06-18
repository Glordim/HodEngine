#pragma once

#include <vector>

namespace hod
{
	class Job;

	/// @brief 
	class FrameStep
	{
	public:

		void				InsertJob(Job* job);
		void				RemoveJob(Job* job);

		void				Enqueue();
		void				Wait();

	private:

		std::vector<Job*>	_jobs;
	};
}
