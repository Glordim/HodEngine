#pragma once
#include "HodEngine/Audio/AudioResource.hpp"
#include "HodEngine/Audio/Export.hpp"

namespace hod::inline audio
{
	class AudioResource;

	class HOD_AUDIO_API AudioInstance
	{
	public:

							AudioInstance(std::shared_ptr<AudioResource> audioResource, bool loop);
							AudioInstance(const AudioInstance&) = delete;
							AudioInstance(AudioInstance&&) = delete;
							~AudioInstance() = default;

		AudioInstance&	operator = (const AudioInstance&) = delete;
		AudioInstance&	operator = (AudioInstance&&) = delete;

	public:

		void	Pause();
		void	Resume();

		void	FillBuffer(float* buffer, uint32_t sampleRate, uint32_t channelCount, uint32_t framesCount);
		bool	IsFinished() const { return _finished; }

	public:

		std::shared_ptr<AudioResource> _audioResource;
		float					_time = 0.0f;
		float					_speed = 1.0f;
		bool					_paused = false;
		bool					_finished = false;
		bool					_loop = false;
	};
}