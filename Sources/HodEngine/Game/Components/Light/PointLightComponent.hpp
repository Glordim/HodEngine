#pragma once

#include <HodEngine/Core/Color.hpp>

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Renderer/Light/PointLight.hpp>

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

									PointLightComponent() = default;
									PointLightComponent(const PointLightComponent&) = delete;
									PointLightComponent(PointLightComponent&&) = delete;
									~PointLightComponent() override = default;

			void					operator=(const PointLightComponent&) = delete;
			void					operator=(PointLightComponent&&) = delete;

		public:

			renderer::PointLight*	GetPointLight() const;

		public:

			renderer::PointLight	_data;
		};
	}
}
