#pragma once

#include <Core/src/Color.h>

#include "../../Component.h"

#include <Renderer/src/Light/SpotLight.h>

namespace HOD
{
	namespace GAME
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

			RENDERER::SpotLight*	GetSpotLight() const;

		public:

			RENDERER::SpotLight		_data;
		};
	}
}
