#pragma once
#include "HodEngine/RendererComponents/Export.hpp"

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Renderer/Light/PointLight.hpp>

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_COMPONENTS_API PointLightComponent : public Component
	{
	public:

								PointLightComponent() = default;
								PointLightComponent(const PointLightComponent&) = delete;
								PointLightComponent(PointLightComponent&&) = delete;
								~PointLightComponent() override = default;

		void					operator=(const PointLightComponent&) = delete;
		void					operator=(PointLightComponent&&) = delete;

	public:

		PointLight*	GetPointLight() const;

	public:

		PointLight	_data;
	};
}
