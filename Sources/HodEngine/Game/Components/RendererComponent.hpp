#pragma once
#include "HodEngine/Game/Export.hpp"

#include <vector>

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Rect.hpp>

namespace hod::renderer
{
	class RenderQueue;
}

namespace hod::game
{
	class CameraComponent;
	class PointLightComponent;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_GAME_API RendererComponent : public Component
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

		virtual void	PushToRenderQueue(renderer::RenderQueue& renderQueue) = 0;
		virtual Rect	GetBoundingBox() const = 0;
	};
}
