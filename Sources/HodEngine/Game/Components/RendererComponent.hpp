#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Math/Rect.hpp>

namespace hod::inline renderer
{
	class RenderView;
}

namespace hod::inline game
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

		virtual void	PushRenderCommand(RenderView& renderView) = 0;
		virtual Rect	GetBoundingBox() const = 0;
	};
}
