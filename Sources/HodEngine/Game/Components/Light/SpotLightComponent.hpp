#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Color.hpp>

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Renderer/Light/SpotLight.hpp>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_GAME_API SpotLightComponent : public Component
		{
		public:

									SpotLightComponent() = default;
									SpotLightComponent(const SpotLightComponent&) = delete;
									SpotLightComponent(SpotLightComponent&&) = delete;
									~SpotLightComponent() override = default;

			void					operator=(const SpotLightComponent&) = delete;
			void					operator=(SpotLightComponent&&) = delete;

		public:

			renderer::SpotLight*	GetSpotLight() const;

		public:

			renderer::SpotLight		_data;
		};
	}
}
