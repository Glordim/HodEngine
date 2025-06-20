#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Core/Frame/FrameStep.hpp"

#include <cstdint>

namespace hod
{
	class Job;

	/// @brief 
	class HOD_CORE_API FrameSequencer : public Singleton<FrameSequencer>
	{
		friend class Singleton<FrameSequencer>;
		friend class Allocator;

	public:

		enum class Step
		{
			PreLogic = 0,
			Logic,
			PostLogic,

			PreRender,
			Render,
			PostRender,

			Count
		};

	public:

		void		InsertJob(Job* job, Step step);
		void		RemoveJob(Job* job, Step step);

		void		EnqueueAndWaitJobs();

	protected:

					FrameSequencer();
					~FrameSequencer() override;

	private:

		FrameStep	_steps[static_cast<uint8_t>(Step::Count)];
	};
}
