#pragma once
#include "HodEngine/Core/Export.hpp"

#include <vector>

namespace hod
{
	class Job;

	/// @brief 
	class HOD_CORE_API FrameStep
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
