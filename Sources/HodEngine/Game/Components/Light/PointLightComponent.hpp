#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Renderer/Light/PointLight.hpp>

namespace hod::inline game
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_GAME_API PointLightComponent : public Component
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
