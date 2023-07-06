#pragma once

#include <HodEngine/Core/Color.h>

#include "HodEngine/Game/src/Component.h"

#include <HodEngine/Renderer/Light/SpotLight.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class SpotLightComponent : public Component
		{
		public:

									SpotLightComponent() = default;
									SpotLightComponent(const SpotLightComponent&) = delete;
									SpotLightComponent(SpotLightComponent&&) = delete;
									~SpotLightComponent() override = default;

			void					operator=(const SpotLightComponent&) = delete;
			void					operator=(SpotLightComponent&&) = delete;

		public:

			const char*				GetType() const override;

			renderer::SpotLight*	GetSpotLight() const;

		public:

			renderer::SpotLight		_data;
		};
	}
}
