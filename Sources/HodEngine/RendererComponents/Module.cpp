#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/Module.hpp"

#include <HodEngine/Game/GameRegistrar.hpp>
#include "HodEngine/RendererComponents/RendererWorldSystem.hpp"

#include "HodEngine/RendererComponents/Components/CameraComponent.hpp"
#include "HodEngine/RendererComponents/Components/Primitive/BoxComponent.hpp"
#include "HodEngine/RendererComponents/Components/TextRendererComponent.hpp"
#include "HodEngine/RendererComponents/Components/TextureRendererComponent.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace
{
	/// @brief The Renderer itself (the GPU device, the frames) is the application's, created before the modules: nothing to init here
	void RegisterRendererModule(hod::ModuleRegistration& registration)
	{
		hod::game::GameRegistrar gameRegistrar(registration);
		gameRegistrar.RegisterComponent<hod::renderer::BoxComponent>();
		gameRegistrar.RegisterComponent<hod::renderer::TextureRendererComponent>();
		gameRegistrar.RegisterComponent<hod::renderer::TextRendererComponent>();
		gameRegistrar.RegisterComponent<hod::renderer::CameraComponent>();
		// Not usable yet: SpriteRendererComponent, PointLightComponent, DirLightComponent, SpotLightComponent

		gameRegistrar.RegisterWorldSystem(hod::renderer::RendererWorldSystem::GetType());
	}
}

HOD_DEFINE_RUNTIME_MODULE(RendererComponents, &RegisterRendererModule)
