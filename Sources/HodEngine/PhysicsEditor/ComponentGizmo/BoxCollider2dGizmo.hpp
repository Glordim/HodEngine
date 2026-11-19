#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/PhysicsComponents/Components/BoxCollider2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief The box collider's outline, for the active entity
	class HOD_PHYSICS_EDITOR_API BoxCollider2dGizmo : public ComponentGizmo<BoxCollider2dComponent>
	{
	protected:
		bool OnDraw(BoxCollider2dComponent& boxCollider2d, GizmoViewport& viewport, bool active) override;
	};
}
