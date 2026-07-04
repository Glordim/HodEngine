#pragma once
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"

namespace hod::inline audio
{
	class HOD_AUDIO_API AudioResource : public Resource
	{
		REFLECTED_CLASS(AudioResource, Resource)

	public:

							AudioResource() = default;
							AudioResource(const AudioResource&) = delete;
							AudioResource(AudioResource&&) = delete;
							~AudioResource() override;

		AudioResource&	operator = (const AudioResource&) = delete;
		AudioResource&	operator = (AudioResource&&) = delete;

	public:

		bool				Initialize(const ResourceContainer& resourceContainer) override;

	public:

		Vector<float>		_samples;
		uint32_t			_sampleRate = 0;
		uint32_t			_channelCount = 0;
		uint32_t			_frameCount = 0;
	};
}