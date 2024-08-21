#include "HodEngine/Audio/AudioManager.hpp"
#include "HodEngine/Core/Frame/FrameSequencer.hpp"

namespace hod::audio
{
    _SingletonConstructor(AudioManager)
	: _updateJob(this, &AudioManager::Update, JobQueue::FramedHighPriority)
	{

	}

	/// @brief 
	void AudioManager::InsertUpdateJob()
	{
		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::Render);
	}

    /// @brief 
    void AudioManager::RemoveUpdateJob()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_updateJob, FrameSequencer::Step::Render);
	}
}
