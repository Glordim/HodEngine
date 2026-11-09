#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/Module.hpp"
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
	void RegisterRendererModule(hod::game::RuntimeModuleRegistration& registration)
	{
		registration.RegisterComponent<hod::renderer::BoxComponent>();
		registration.RegisterComponent<hod::renderer::TextureRendererComponent>();
		registration.RegisterComponent<hod::renderer::TextRendererComponent>();
		registration.RegisterComponent<hod::renderer::CameraComponent>();
		// Not usable yet: SpriteRendererComponent, PointLightComponent, DirLightComponent, SpotLightComponent

		registration.RegisterWorldSystem(hod::renderer::RendererWorldSystem::GetType());
	}
}

HOD_DEFINE_RUNTIME_MODULE(RendererComponents, &RegisterRendererModule)
