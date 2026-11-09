#pragma once
#include "HodEngine/RendererComponents/Export.hpp"

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Renderer/Light/SpotLight.hpp>

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_COMPONENTS_API SpotLightComponent : public Component
	{
	public:

								SpotLightComponent() = default;
								SpotLightComponent(const SpotLightComponent&) = delete;
								SpotLightComponent(SpotLightComponent&&) = delete;
								~SpotLightComponent() override = default;

		void					operator=(const SpotLightComponent&) = delete;
		void					operator=(SpotLightComponent&&) = delete;

	public:

		SpotLight*	GetSpotLight() const;

	public:

		SpotLight		_data;
	};
}
