#include "HodEngine/Audio/Pch.hpp"
#include <HodEngine/Core/RuntimeModule.hpp>
#include "HodEngine/Audio/PlatformAudioManager.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline audio
{
	/// @brief The platform's audio device (its components are AudioComponents')
	HOD_STARTUP_RUNTIME_MODULE(Audio, registration)
	{
		registration.AddInit(
			[]()
			{
				PlatformAudioManager::CreateInstance();
				return PlatformAudioManager::GetInstance()->Initialize();
			},
			[]() { PlatformAudioManager::DestroyInstance(); });
	}
}
