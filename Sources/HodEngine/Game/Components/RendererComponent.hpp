#pragma once

#include <vector>

#include "HodEngine/Game/Component.hpp"

namespace hod
{
	namespace game
	{
		class CameraComponent;
		class PointLightComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RendererComponent : public Component
		{
			REFLECTED_CLASS(RendererComponent, Component)

		public:

							RendererComponent() = default;
							RendererComponent(const RendererComponent&) = delete;
							RendererComponent(RendererComponent&&) = delete;
							~RendererComponent() override = default;

			void			operator=(const RendererComponent&) = delete;
			void			operator=(RendererComponent&&) = delete;

		public:

			//virtual void	Render() = 0;
		};
	}
}
