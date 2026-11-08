#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/Module.hpp"
#include "HodEngine/Audio/PlatformAudioManager.hpp"

#include "HodEngine/Audio/Components/AudioSourceComponent.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace
{
	void RegisterAudioModule(hod::game::RuntimeModuleRegistration& registration)
	{
		registration.RegisterComponent<hod::audio::AudioSourceComponent>();
	}

	/// @brief The platform's audio device
	bool InitAudioModule()
	{
		PlatformAudioManager::CreateInstance();
		return PlatformAudioManager::GetInstance()->Initialize();
	}

	void ShutdownAudioModule()
	{
		PlatformAudioManager::DestroyInstance();
	}
}

HOD_DEFINE_RUNTIME_MODULE(Audio, &RegisterAudioModule, &InitAudioModule, &ShutdownAudioModule)
