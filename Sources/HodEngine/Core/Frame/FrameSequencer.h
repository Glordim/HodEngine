#pragma once

#include "HodEngine/Core/Singleton.h"
#include "HodEngine/Core/Frame/FrameStep.h"

#include <cstdint>

namespace hod
{
	class Job;

	/// @brief 
	class FrameSequencer : public Singleton<FrameSequencer>
	{
		friend class Singleton<FrameSequencer>;

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
