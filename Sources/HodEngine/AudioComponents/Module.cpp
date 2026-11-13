#include "HodEngine/AudioComponents/Pch.hpp"
#include "HodEngine/AudioComponents/Module.hpp"

#include <HodEngine/Game/GameRegistrar.hpp>
#include "HodEngine/Audio/PlatformAudioManager.hpp"

#include "HodEngine/AudioComponents/Components/AudioSourceComponent.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline audio
{
	namespace
	{
		void RegisterAudioModule(ModuleRegistration& registration)
		{
			GameRegistrar gameRegistrar(registration);
			gameRegistrar.RegisterComponent<AudioSourceComponent>();
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

	HOD_DEFINE_RUNTIME_MODULE(AudioComponents, &RegisterAudioModule, &InitAudioModule, &ShutdownAudioModule)
}
