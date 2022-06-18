#pragma once

#include <HodEngine/Core/Src/Color.h>

#include "../../Component.h"

#include <HodEngine/Renderer/src/Light/SpotLight.h>

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

									SpotLightComponent(Actor* actor);
									SpotLightComponent(const SpotLightComponent&) = delete;
									SpotLightComponent(SpotLightComponent&&) = delete;
									~SpotLightComponent() override = default;

			void					operator=(const SpotLightComponent&) = delete;
			void					operator=(SpotLightComponent&&) = delete;

		public:

			void					DrawImGui() override;
			const char*				GetType() const override;

			renderer::SpotLight*	GetSpotLight() const;

		public:

			renderer::SpotLight		_data;
		};
	}
}
