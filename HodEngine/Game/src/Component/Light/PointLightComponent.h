#pragma once

#include <Core/src/Color.h>

#include "../../Component.h"

#include <Renderer/src/Light/PointLight.h>

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

			void			operator=(const PointLightComponent&) = delete;
			void			operator=(PointLightComponent&&) = delete;

		public:

			void			DrawImGui() override;
			const char*		GetType() const override;

			PointLight*		GetPointLight() const;

		public:
			PointLight		_data;
		};
	}
}
