#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/RendererComponents/Components/TextureRendererComponent.hpp>

namespace hod::inline editor
{
	/// @brief The texture renderer's bounds, for the active entity
	class HOD_RENDERER_EDITOR_API TextureRendererGizmo : public ComponentGizmo<TextureRendererComponent>
	{
	protected:
		bool OnDraw(TextureRendererComponent& textureRenderer, GizmoViewport& viewport, bool active) override;
	};
}
