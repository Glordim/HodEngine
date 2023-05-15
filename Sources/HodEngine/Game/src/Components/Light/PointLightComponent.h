#pragma once

#include <HodEngine/Core/Color.h>

#include "../../Component.h"

#include <HodEngine/Renderer/Light/PointLight.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class PointLightComponent : public Component
		{
		public:

									PointLightComponent(Actor* actor);
									PointLightComponent(const PointLightComponent&) = delete;
									PointLightComponent(PointLightComponent&&) = delete;
									~PointLightComponent() override = default;

			void					operator=(const PointLightComponent&) = delete;
			void					operator=(PointLightComponent&&) = delete;

		public:

			void					DrawImGui() override;
			const char*				GetType() const override;

			renderer::PointLight*	GetPointLight() const;

		public:

			renderer::PointLight	_data;
		};
	}
}
