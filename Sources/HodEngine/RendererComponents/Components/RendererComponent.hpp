#pragma once
#include "HodEngine/RendererComponents/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/RequiredComponent.hpp"

#include <HodEngine/Math/Rect.hpp>

namespace hod::inline renderer
{
	class RenderView;
}

namespace hod::inline game
{
	class Node2dComponent;
}

namespace hod::inline renderer
{
	class CameraComponent;
	class PointLightComponent;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_COMPONENTS_API RendererComponent : public Component
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

	protected:

		RequiredComponent<Node2dComponent>	_node2d; // Drawn where its node is
	};
}
