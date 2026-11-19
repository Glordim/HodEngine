#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/RendererComponents/Components/CameraComponent.hpp>

namespace hod::inline editor
{
	/// @brief The camera's frame at the viewport's play ratio, for the active entity
	class HOD_RENDERER_EDITOR_API CameraGizmo : public ComponentGizmo<CameraComponent>
	{
	protected:
		bool OnDraw(CameraComponent& camera, GizmoViewport& viewport, bool active) override;
	};
}
