#include "HodEngine/AudioComponents/Pch.hpp"
#include <HodEngine/Core/RuntimeModule.hpp>

#include "HodEngine/AudioComponents/Components/AudioSourceComponent.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline audio
{
	/// @brief The components (the audio device is Audio's)
	HOD_STARTUP_RUNTIME_MODULE(AudioComponents, registration)
	{
		GameRegistrar gameRegistrar(registration);
		gameRegistrar.RegisterComponent<AudioSourceComponent>();
		return true;
	}
}
