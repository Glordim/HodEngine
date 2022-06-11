#pragma once

#include <HodEngine/Core/Src/Color.h>

#include "../../Component.h"

#include <HodEngine/Renderer/src/Light/PointLight.h>

namespace HOD
{
	namespace GAME
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

			RENDERER::PointLight*	GetPointLight() const;

		public:

			RENDERER::PointLight	_data;
		};
	}
}
