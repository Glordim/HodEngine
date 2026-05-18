#pragma once
#include "HodEngine/Audio/AudioManager.hpp"
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"

#include <cstdint>

typedef struct AAudioStreamStruct AAudioStream;

namespace hod::inline audio
{
	class HOD_AUDIO_API AndroidAudioManager : public AudioManager
	{
		_SingletonOverride(AndroidAudioManager)

	public:
		bool Initialize() override;
		void Terminate() override;

		void Update() override;

	private:
		static int32_t DataCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames);

	private:
		AAudioStream*	_stream = nullptr;
		int32_t			_sampleRate = 0;
		int32_t			_channelCount = 0;
	};
}
