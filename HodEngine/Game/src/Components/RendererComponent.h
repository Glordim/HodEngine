#pragma once

#include <vector>

#include "../Component.h"

namespace HOD
{
	namespace GAME
	{
		class CameraComponent;
		class PointLightComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RendererComponent : public Component
		{
			REFLECTED_ABSTRACT_DERIVED_CLASS(RendererComponent, Component)

		public:

							RendererComponent(Actor* actor);
							RendererComponent(const RendererComponent&) = delete;
							RendererComponent(RendererComponent&&) = delete;
							~RendererComponent() override = default;

			void			operator=(const RendererComponent&) = delete;
			void			operator=(RendererComponent&&) = delete;

		public:

			void			DrawImGui() override;
			const char*		GetType() const override;
		};
	}
}
