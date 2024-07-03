#pragma once
#include <HodEngine/HodEngine.hpp>

#include <vector>

namespace hod
{
	class Job;

	/// @brief 
	class HOD_API FrameStep
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
