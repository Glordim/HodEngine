#pragma once
#include "HodEngine/RendererComponents/Export.hpp"

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Renderer/Light/DirLight.hpp>

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_COMPONENTS_API DirLightComponent : public Component
	{
	public:

							DirLightComponent() = default;
							DirLightComponent(const DirLightComponent&) = delete;
							DirLightComponent(DirLightComponent&&) = delete;
							~DirLightComponent() override = default;

		void				operator=(const DirLightComponent&) = delete;
		void				operator=(DirLightComponent&&) = delete;

	public:

		DirLight*	GetDirLight() const;

	public:
		DirLight	_data;
	};
}
