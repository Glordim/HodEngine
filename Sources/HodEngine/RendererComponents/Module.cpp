#include "HodEngine/RendererComponents/Pch.hpp"
#include <HodEngine/Core/Module.hpp>

#include <HodEngine/Game/GameRegistrar.hpp>
#include "HodEngine/RendererComponents/RendererWorldSystem.hpp"

#include "HodEngine/RendererComponents/Components/CameraComponent.hpp"
#include "HodEngine/RendererComponents/Components/Primitive/BoxComponent.hpp"
#include "HodEngine/RendererComponents/Components/TextRendererComponent.hpp"
#include "HodEngine/RendererComponents/Components/TextureRendererComponent.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline renderer
{
	/// @brief The components and the world system. The Renderer itself (the GPU device, the frames) is the application's, created before the modules
	HOD_STARTUP_MODULE(RendererComponents, registration)
	{
		GameRegistrar gameRegistrar(registration);
		gameRegistrar.RegisterComponent<BoxComponent>();
		gameRegistrar.RegisterComponent<TextureRendererComponent>();
		gameRegistrar.RegisterComponent<TextRendererComponent>();
		gameRegistrar.RegisterComponent<CameraComponent>();
		// Not usable yet: SpriteRendererComponent, PointLightComponent, DirLightComponent, SpotLightComponent

		gameRegistrar.RegisterWorldSystem(RendererWorldSystem::GetType());
		return true;
	}
}
