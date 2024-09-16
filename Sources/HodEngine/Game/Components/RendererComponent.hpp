#pragma once
#include "HodEngine/Game/Export.hpp"

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
		class HOD_GAME_API RendererComponent : public Component
		{
			REFLECTED_CLASS(RendererComponent, Component, HOD_GAME_API)

		public:

							RendererComponent() = default;
							RendererComponent(const RendererComponent&) = delete;
							RendererComponent(RendererComponent&&) = delete;
							~RendererComponent() override = default;

			void			operator=(const RendererComponent&) = delete;
			void			operator=(RendererComponent&&) = delete;

		public:

			virtual void	PushToRenderQueue(renderer::RenderQueue& renderQueue) {};
		};
	}
}
