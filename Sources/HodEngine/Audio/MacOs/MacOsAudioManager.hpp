#pragma once
#include "HodEngine/Audio/AudioManager.hpp"
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"

namespace hod::audio
{
	class HOD_AUDIO_API MacOsAudioManager : public AudioManager
	{
		_SingletonOverride(MacOsAudioManager)

	public:
		bool Initialize() override;
		void Terminate() override;

		void Update() override;
	};
}
